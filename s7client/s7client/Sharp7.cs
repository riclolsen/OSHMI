/*=============================================================================|
|  PROJECT Sharp7                                                        1.0.0 |
|==============================================================================|
|  Copyright (C) 2016 Davide Nardella                                          |
|  All rights reserved.                                                        |
|==============================================================================|
|  Sharp7 is free software: you can redistribute it and/or modify              |
|  it under the terms of the Lesser GNU General Public License as published by |
|  the Free Software Foundation, either version 3 of the License, or           |
|  (at your option) any later version.                                         |
|                                                                              |
|  It means that you can distribute your commercial software which includes    |
|  Sharp7 without the requirement to distribute the source code of your        |
|  application and without the requirement that your application be itself     |
|  distributed under LGPL.                                                     |
|                                                                              |
|  Sharp7 is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
|  Lesser GNU General Public License for more details.                         |
|                                                                              |
|  You should have received a copy of the GNU General Public License and a     |
|  copy of Lesser GNU General Public License along with Sharp7.                |
|  If not, see  http://www.gnu.org/licenses/                                   |
|=============================================================================*/
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
//------------------------------------------------------------------------------
// If you are compiling for UWP verify that WINDOWS_UWP or NETFX_CORE are 
// defined into Project Properties->Build->Conditional compilation symbols
//------------------------------------------------------------------------------
#if WINDOWS_UWP || NETFX_CORE
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
#else // <-- Including MONO
using System.Net.Sockets;
#endif

namespace Sharp7
{

    #region [Async Sockets UWP(W10,IoT,Phone)/Windows 8/Windows 8 Phone]
#if WINDOWS_UWP || NETFX_CORE
    class MsgSocket
    {
        private DataReader Reader = null;
        private DataWriter Writer = null;
        private StreamSocket TCPSocket;

        private bool _Connected;

        private int _ReadTimeout = 2000;
        private int _WriteTimeout = 2000;
        private int _ConnectTimeout = 1000;

        public static int LastError = 0;


        private void CreateSocket()
        {
            TCPSocket = new StreamSocket();
            TCPSocket.Control.NoDelay = true;
            _Connected = false;
        }

        public MsgSocket()
        {
        }

        public void Close()
        {
            if (Reader != null)
            {
                Reader.Dispose();
                Reader = null;
            }
            if (Writer != null)
            {
                Writer.Dispose();
                Writer = null;
            }
            if (TCPSocket != null)
            {
                TCPSocket.Dispose();
                TCPSocket = null;
            }
            _Connected = false;
        }

        private async Task AsConnect(string Host, string port, CancellationTokenSource cts)
        {
            HostName ServerHost = new HostName(Host);
            try
            {
                await TCPSocket.ConnectAsync(ServerHost, port).AsTask(cts.Token);
                _Connected = true;
            }
            catch (TaskCanceledException)
            {
                LastError = S7Consts.errTCPConnectionTimeout;
            }
            catch
            {
                LastError = S7Consts.errTCPConnectionFailed; // Maybe unreachable peer
            }
        }

        public int Connect(string Host, int Port)
        {
            LastError = 0;
            if (!Connected)
            {
                CreateSocket();
                CancellationTokenSource cts = new CancellationTokenSource();
                try
                {
                    try
                    {
                        cts.CancelAfter(_ConnectTimeout);
                        Task.WaitAny(Task.Run(async () => await AsConnect(Host, Port.ToString(), cts)));
                    }
                    catch
                    {
                        LastError = S7Consts.errTCPConnectionFailed;
                    }
                }
                finally
                {
                    if (cts != null)
                    {
                        try
                        {
                            cts.Cancel();
                            cts.Dispose();
                            cts = null;
                        }
                        catch { }
                    }

                }
                if (LastError == 0)
                {
                    Reader = new DataReader(TCPSocket.InputStream);
                    Reader.InputStreamOptions = InputStreamOptions.Partial;
                    Writer = new DataWriter(TCPSocket.OutputStream);
                    _Connected = true;
                }
                else
                    Close();
            }
            return LastError;
        }

        private async Task AsReadBuffer(byte[] Buffer, int Size, CancellationTokenSource cts)
        {
            try
            {
                await Reader.LoadAsync((uint)Size).AsTask(cts.Token);
                Reader.ReadBytes(Buffer);
            }
            catch
            {
                LastError = S7Consts.errTCPDataReceive;
            }
        }

        public int Receive(byte[] Buffer, int Start, int Size)
        {
            byte[] InBuffer = new byte[Size];
            CancellationTokenSource cts = new CancellationTokenSource();
            LastError = 0;
            try
            {
                try
                {
                    cts.CancelAfter(_ReadTimeout);
                    Task.WaitAny(Task.Run(async () => await AsReadBuffer(InBuffer, Size, cts)));
                }
                catch
                {
                    LastError = S7Consts.errTCPDataReceive;
                }
            }
            finally
            {
                if (cts != null)
                {
                    try
                    {
                        cts.Cancel();
                        cts.Dispose();
                        cts = null;
                    }
                    catch { }
                }
            }
            if (LastError == 0)
                Array.Copy(InBuffer, 0, Buffer, Start, Size);
            else
                Close();
            return LastError;
        }

        private async Task WriteBuffer(byte[] Buffer, CancellationTokenSource cts)
        {
            try
            {
                Writer.WriteBytes(Buffer);
                await Writer.StoreAsync().AsTask(cts.Token);
            }
            catch
            {
                LastError = S7Consts.errTCPDataSend;
            }
        }

        public int Send(byte[] Buffer, int Size)
        {
            byte[] OutBuffer = new byte[Size];
            CancellationTokenSource cts = new CancellationTokenSource();
            Array.Copy(Buffer, 0, OutBuffer, 0, Size);
            LastError = 0;
            try
            {
                try
                {
                    cts.CancelAfter(_WriteTimeout);
                    Task.WaitAny(Task.Run(async () => await WriteBuffer(OutBuffer, cts)));
                }
                catch
                {
                    LastError = S7Consts.errTCPDataSend;
                }
            }
            finally
            {
                if (cts != null)
                {
                    try
                    {
                        cts.Cancel();
                        cts.Dispose();
                        cts = null;
                    }
                    catch { }
                }
            }
            if (LastError != 0)
                Close();
            return LastError;
        }

        ~MsgSocket()
        {
            Close();
        }

        public bool Connected
        {
            get
            {
                return (TCPSocket != null) && _Connected;
            }
        }

        public int ReadTimeout
        {
            get
            {
                return _ReadTimeout;
            }
            set
            {
                _ReadTimeout = value;
            }
        }

        public int WriteTimeout
        {
            get
            {
                return _WriteTimeout;
            }
            set
            {
                _WriteTimeout = value;
            }
        }
        public int ConnectTimeout
        {
            get
            {
                return _ConnectTimeout;
            }
            set
            {
                _ConnectTimeout = value;
            }
        }
    }
#endif
    #endregion

    #region [Sync Sockets Win32/Win64 Desktop Application]
#if !WINDOWS_UWP && !NETFX_CORE
    class MsgSocket
    {
        private Socket TCPSocket;
        private int _ReadTimeout = 2000;
        private int _WriteTimeout = 2000;
        private int _ConnectTimeout = 1000;
        public int LastError = 0;

        public MsgSocket()
        {
        }

        ~MsgSocket()
        {
            Close();
        }

        public void Close()
        {
            if (TCPSocket != null)
            {
                TCPSocket.Dispose();
                TCPSocket = null;
            }
        }

        private void CreateSocket()
        {
            TCPSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            TCPSocket.NoDelay = true;
        }

        private void TCPPing(string Host, int Port)
        {
            // To Ping the PLC an Asynchronous socket is used rather then an ICMP packet.
            // This allows the use also across Internet and Firewalls (obviously the port must be opened)           
            LastError = 0;
            Socket PingSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                IAsyncResult result = PingSocket.BeginConnect(Host, Port, null, null);
                bool success = result.AsyncWaitHandle.WaitOne(_ConnectTimeout, true);
                if (!success)
                {
                    LastError = S7Consts.errTCPConnectionFailed;
                }
            }
            catch
            {
                LastError = S7Consts.errTCPConnectionFailed;
            };
            PingSocket.Close();
        }

        public int Connect(string Host, int Port)
        {
            LastError = 0;
            if (!Connected)
            { 
                TCPPing(Host, Port);
                if (LastError == 0)
                try
                {
                        CreateSocket();
                        TCPSocket.Connect(Host, Port);
                }
                catch
                {
                        LastError = S7Consts.errTCPConnectionFailed;
                }
            }
            return LastError;
        }

        private int WaitForData(int Size, int Timeout)
        {
            bool Expired = false;
            int SizeAvail;
            int Elapsed = Environment.TickCount;
            LastError = 0;
            try
            {
                SizeAvail = TCPSocket.Available;
                while ((SizeAvail < Size) && (!Expired))
                {
                    Thread.Sleep(2);
                    SizeAvail = TCPSocket.Available;
                    Expired = Environment.TickCount - Elapsed > Timeout;
                    // If timeout we clean the buffer
                    if (Expired && (SizeAvail > 0))
                    try
                    {
                        byte[] Flush = new byte[SizeAvail];
                        TCPSocket.Receive(Flush, 0, SizeAvail, SocketFlags.None);
                    }
                    catch { }
                }
            }
            catch
            {
                LastError = S7Consts.errTCPDataReceive;
            }
            if (Expired)
            {
                LastError = S7Consts.errTCPDataReceive;
            }
            return LastError;
        }

        public int Receive(byte[] Buffer, int Start, int Size)
        {

            int BytesRead = 0;
            LastError = WaitForData(Size, _ReadTimeout);
            if (LastError == 0)
            {
                try
                {
                    BytesRead = TCPSocket.Receive(Buffer, Start, Size, SocketFlags.None);
                }
                catch
                {
                    LastError = S7Consts.errTCPDataReceive;
                }
                if (BytesRead == 0) // Connection Reset by the peer
                {
                    LastError = S7Consts.errTCPDataReceive;
                    Close();
                }
            }
            return LastError;
        }

        public int Send(byte[] Buffer, int Size)
        {
            LastError = 0;
            try
            {
                int BytesSent = TCPSocket.Send(Buffer, Size, SocketFlags.None);
            }
            catch
            {
                LastError = S7Consts.errTCPDataSend;
                Close();
            }
            return LastError;
        }

        public bool Connected
        {
            get
            {
                return (TCPSocket != null) && (TCPSocket.Connected); 
            }
        }

        public int ReadTimeout
        {
            get
            {
                return _ReadTimeout;
            }
            set
            {
                _ReadTimeout = value;
            }
        }

        public int WriteTimeout
        {
            get
            {
                return _WriteTimeout;
            }
            set
            {
                _WriteTimeout = value;
            }

        }
        public int ConnectTimeout
        {
            get
            {
                return _ConnectTimeout;
            }
            set
            {
                _ConnectTimeout = value;
            }
        }
    }   
#endif
#endregion

    public static class S7Consts
    {
        #region [Exported Consts]
        // Error codes
        //------------------------------------------------------------------------------
        //                                     ERRORS                 
        //------------------------------------------------------------------------------
        public const int errTCPSocketCreation         = 0x00000001;
        public const int errTCPConnectionTimeout      = 0x00000002;
        public const int errTCPConnectionFailed       = 0x00000003;
        public const int errTCPReceiveTimeout         = 0x00000004;
        public const int errTCPDataReceive            = 0x00000005;
        public const int errTCPSendTimeout            = 0x00000006;
        public const int errTCPDataSend               = 0x00000007;
        public const int errTCPConnectionReset        = 0x00000008;
        public const int errTCPNotConnected           = 0x00000009;
        public const int errTCPUnreachableHost        = 0x00002751;

        public const int errIsoConnect                = 0x00010000; // Connection error
        public const int errIsoInvalidPDU             = 0x00030000; // Bad format
        public const int errIsoInvalidDataSize        = 0x00040000; // Bad Datasize passed to send/recv : buffer is invalid

        public const int errCliNegotiatingPDU         = 0x00100000;
        public const int errCliInvalidParams          = 0x00200000;
        public const int errCliJobPending             = 0x00300000;
        public const int errCliTooManyItems           = 0x00400000;
        public const int errCliInvalidWordLen         = 0x00500000;
        public const int errCliPartialDataWritten     = 0x00600000;
        public const int errCliSizeOverPDU            = 0x00700000;
        public const int errCliInvalidPlcAnswer       = 0x00800000;
        public const int errCliAddressOutOfRange      = 0x00900000;
        public const int errCliInvalidTransportSize   = 0x00A00000;
        public const int errCliWriteDataSizeMismatch  = 0x00B00000;
        public const int errCliItemNotAvailable       = 0x00C00000;
        public const int errCliInvalidValue           = 0x00D00000;
        public const int errCliCannotStartPLC         = 0x00E00000;
        public const int errCliAlreadyRun             = 0x00F00000;
        public const int errCliCannotStopPLC          = 0x01000000;
        public const int errCliCannotCopyRamToRom     = 0x01100000;
        public const int errCliCannotCompress         = 0x01200000;
        public const int errCliAlreadyStop            = 0x01300000;
        public const int errCliFunNotAvailable        = 0x01400000;
        public const int errCliUploadSequenceFailed   = 0x01500000;
        public const int errCliInvalidDataSizeRecvd   = 0x01600000;
        public const int errCliInvalidBlockType       = 0x01700000;
        public const int errCliInvalidBlockNumber     = 0x01800000;
        public const int errCliInvalidBlockSize       = 0x01900000;
        public const int errCliNeedPassword           = 0x01D00000;
        public const int errCliInvalidPassword        = 0x01E00000;
        public const int errCliNoPasswordToSetOrClear = 0x01F00000;
        public const int errCliJobTimeout             = 0x02000000;
        public const int errCliPartialDataRead        = 0x02100000;
        public const int errCliBufferTooSmall         = 0x02200000;
        public const int errCliFunctionRefused        = 0x02300000;
        public const int errCliDestroying             = 0x02400000;
        public const int errCliInvalidParamNumber     = 0x02500000;
        public const int errCliCannotChangeParam      = 0x02600000;
        public const int errCliFunctionNotImplemented = 0x02700000;
        //------------------------------------------------------------------------------
        //        PARAMS LIST FOR COMPATIBILITY WITH Snap7.net.cs           
        //------------------------------------------------------------------------------
        public const Int32 p_u16_LocalPort     = 1;  // Not applicable here
        public const Int32 p_u16_RemotePort    = 2;
        public const Int32 p_i32_PingTimeout   = 3;
        public const Int32 p_i32_SendTimeout   = 4;
        public const Int32 p_i32_RecvTimeout   = 5;
        public const Int32 p_i32_WorkInterval  = 6;  // Not applicable here
        public const Int32 p_u16_SrcRef        = 7;  // Not applicable here
        public const Int32 p_u16_DstRef        = 8;  // Not applicable here
        public const Int32 p_u16_SrcTSap       = 9;  // Not applicable here
        public const Int32 p_i32_PDURequest    = 10;
        public const Int32 p_i32_MaxClients    = 11; // Not applicable here
        public const Int32 p_i32_BSendTimeout  = 12; // Not applicable here
        public const Int32 p_i32_BRecvTimeout  = 13; // Not applicable here
        public const Int32 p_u32_RecoveryTime  = 14; // Not applicable here
        public const Int32 p_u32_KeepAliveTime = 15; // Not applicable here
        // Area ID
        public const byte S7AreaPE = 0x81;
        public const byte S7AreaPA = 0x82;
        public const byte S7AreaMK = 0x83;
        public const byte S7AreaDB = 0x84;
        public const byte S7AreaCT = 0x1C;
        public const byte S7AreaTM = 0x1D;
        // Word Length
        public const int S7WLBit     = 0x01;
        public const int S7WLByte    = 0x02;
        public const int S7WLChar    = 0x03;
        public const int S7WLWord    = 0x04;
        public const int S7WLInt     = 0x05;
        public const int S7WLDWord   = 0x06;
        public const int S7WLDInt    = 0x07;
        public const int S7WLReal    = 0x08;
        public const int S7WLCounter = 0x1C;
        public const int S7WLTimer   = 0x1D;
        // PLC Status
        public const int S7CpuStatusUnknown = 0x00;
        public const int S7CpuStatusRun     = 0x08;
        public const int S7CpuStatusStop    = 0x04;

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct S7Tag
        {
            public Int32 Area;
            public Int32 DBNumber;
            public Int32 Start;
            public Int32 Elements;
            public Int32 WordLen;
        }
        #endregion
    }

    public static class S7
    {
        #region [Help Functions]

        private static Int64 bias = 621355968000000000; // "decimicros" between 0001-01-01 00:00:00 and 1970-01-01 00:00:00

        private static int BCDtoByte(byte B)
        {
             return ((B >> 4) * 10) + (B & 0x0F);        
        }

        private static byte ByteToBCD(int Value)
        {
            return (byte)(((Value / 10) << 4) | (Value % 10));
        }
        
        private static byte[] CopyFrom(byte[] Buffer, int Pos, int Size)
        {
            byte[] Result=new byte[Size];
            Array.Copy(Buffer, Pos, Result, 0, Size);
            return Result;
        }

        public static int DataSizeByte(int WordLength)
        {
            switch (WordLength)
            {
                case S7Consts.S7WLBit: return 1;  // S7 sends 1 byte per bit
                case S7Consts.S7WLByte: return 1;
                case S7Consts.S7WLChar: return 1;
                case S7Consts.S7WLWord: return 2;
                case S7Consts.S7WLDWord: return 4;
                case S7Consts.S7WLInt: return 2;
                case S7Consts.S7WLDInt: return 4;
                case S7Consts.S7WLReal: return 4;
                case S7Consts.S7WLCounter: return 2;
                case S7Consts.S7WLTimer: return 2;
                default: return 0;
            }
        }

        #region Get/Set the bit at Pos.Bit
        public static bool GetBitAt(byte[] Buffer, int Pos, int Bit)
        {           
            byte[] Mask = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
            if (Bit < 0) Bit = 0;
            if (Bit > 7) Bit = 7;
            return (Buffer[Pos] & Mask[Bit]) != 0;
        }
        public static void SetBitAt(ref byte[] Buffer, int Pos, int Bit, bool Value)
        {
            byte[] Mask = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
            if (Bit < 0) Bit = 0;
            if (Bit > 7) Bit = 7;

            if (Value)
                Buffer[Pos] = (byte)(Buffer[Pos] | Mask[Bit]);
            else
                Buffer[Pos] = (byte)(Buffer[Pos] & ~Mask[Bit]);
        }
        #endregion

        #region Get/Set 8 bit signed value (S7 SInt) -128..127
        public static int GetSIntAt(byte[] Buffer, int Pos)
        {
            int Value = Buffer[Pos];
            if (Value < 128)
                return Value;
            else
                return (int) (Value - 256);
        }
        public static void SetSIntAt(byte[] Buffer, int Pos, int Value)
        {
            if (Value < -128) Value = -128;
            if (Value > 127) Value = 127;
            Buffer[Pos] = (byte)Value;
        }
        #endregion

        #region Get/Set 16 bit signed value (S7 int) -32768..32767
        public static int GetIntAt(byte[] Buffer, int Pos)
        {
            return (int)((Buffer[Pos] << 8) | Buffer[Pos + 1]);
        }
        public static void SetIntAt(byte[] Buffer, int Pos, Int16 Value)
        {
            Buffer[Pos] = (byte)(Value >> 8);
            Buffer[Pos + 1] = (byte)(Value & 0x00FF);
        }
        #endregion

        #region Get/Set 32 bit signed value (S7 DInt) -2147483648..2147483647
        public static int GetDIntAt(byte[] Buffer, int Pos)
        {
            int Result;
            Result = Buffer[Pos]; Result <<= 8;
            Result += Buffer[Pos + 1]; Result <<= 8;
            Result += Buffer[Pos + 2]; Result <<= 8;
            Result += Buffer[Pos + 3];
            return Result;
        }
        public static void SetDIntAt(byte[] Buffer, int Pos, int Value)
        {
            Buffer[Pos + 3] = (byte)(Value & 0xFF);
            Buffer[Pos + 2] = (byte)((Value >> 8) & 0xFF);
            Buffer[Pos + 1] = (byte)((Value >> 16) & 0xFF);
            Buffer[Pos] = (byte)((Value >> 24) & 0xFF);
        }
        #endregion

        #region Get/Set 64 bit signed value (S7 LInt) -9223372036854775808..9223372036854775807
        public static Int64 GetLIntAt(byte[] Buffer, int Pos)
        {
            Int64 Result;
            Result = Buffer[Pos]; Result <<= 8;
            Result += Buffer[Pos + 1]; Result <<= 8;
            Result += Buffer[Pos + 2]; Result <<= 8;
            Result += Buffer[Pos + 3]; Result <<= 8;
            Result += Buffer[Pos + 4]; Result <<= 8;
            Result += Buffer[Pos + 5]; Result <<= 8;
            Result += Buffer[Pos + 6]; Result <<= 8;
            Result += Buffer[Pos + 7]; 
            return Result;
        }
        public static void SetLIntAt(byte[] Buffer, int Pos, Int64 Value)
        {
            Buffer[Pos + 7] = (byte)(Value & 0xFF);
            Buffer[Pos + 6] = (byte)((Value >> 8) & 0xFF);
            Buffer[Pos + 5] = (byte)((Value >> 16) & 0xFF);
            Buffer[Pos + 4] = (byte)((Value >> 24) & 0xFF);
            Buffer[Pos + 3] = (byte)((Value >> 32) & 0xFF);
            Buffer[Pos + 2] = (byte)((Value >> 40) & 0xFF);
            Buffer[Pos + 1] = (byte)((Value >> 48) & 0xFF);
            Buffer[Pos] = (byte)((Value >> 56) & 0xFF);
        }
        #endregion

        #region Get/Set 8 bit unsigned value (S7 USInt) 0..255
        public static byte GetUSIntAt(byte[] Buffer, int Pos)
        {
            return Buffer[Pos];
        }
        public static void SetUSIntAt(byte[] Buffer, int Pos, byte Value)
        {
            Buffer[Pos] = Value;
        }
        #endregion

        #region Get/Set 16 bit unsigned value (S7 UInt) 0..65535
        public static UInt16 GetUIntAt(byte[] Buffer, int Pos)
        {
            return (UInt16)((Buffer[Pos] << 8) | Buffer[Pos + 1]);
        }
        public static void SetUIntAt(byte[] Buffer, int Pos, UInt16 Value)
        {
            Buffer[Pos] = (byte)(Value >> 8);
            Buffer[Pos + 1] = (byte)(Value & 0x00FF);
        }
        #endregion

        #region Get/Set 32 bit unsigned value (S7 UDInt) 0..4294967296
        public static UInt32 GetUDIntAt(byte[] Buffer, int Pos)
        {
            UInt32 Result;
            Result = Buffer[Pos]; Result <<= 8;
            Result |= Buffer[Pos + 1]; Result <<= 8;
            Result |= Buffer[Pos + 2]; Result <<= 8;
            Result |= Buffer[Pos + 3];
            return Result;
        }
        public static void SetUDIntAt(byte[] Buffer, int Pos, UInt32 Value)
        {
            Buffer[Pos + 3] = (byte)(Value & 0xFF);
            Buffer[Pos + 2] = (byte)((Value >> 8) & 0xFF);
            Buffer[Pos + 1] = (byte)((Value >> 16) & 0xFF);
            Buffer[Pos] = (byte)((Value >> 24) & 0xFF);
        }
        #endregion

        #region Get/Set 64 bit unsigned value (S7 ULint) 0..18446744073709551616
        public static UInt64 GetULIntAt(byte[] Buffer, int Pos)
        {
            UInt64 Result;
            Result = Buffer[Pos]; Result <<= 8;
            Result |= Buffer[Pos + 1]; Result <<= 8;
            Result |= Buffer[Pos + 2]; Result <<= 8;
            Result |= Buffer[Pos + 3]; Result <<= 8;
            Result |= Buffer[Pos + 4]; Result <<= 8;
            Result |= Buffer[Pos + 5]; Result <<= 8;
            Result |= Buffer[Pos + 6]; Result <<= 8;
            Result |= Buffer[Pos + 7];                  
            return Result;
        }
        public static void SetULintAt(byte[] Buffer, int Pos, UInt64 Value)
        {
            Buffer[Pos + 7] = (byte)(Value & 0xFF);
            Buffer[Pos + 6] = (byte)((Value >> 8) & 0xFF);
            Buffer[Pos + 5] = (byte)((Value >> 16) & 0xFF);
            Buffer[Pos + 4] = (byte)((Value >> 24) & 0xFF);
            Buffer[Pos + 3] = (byte)((Value >> 32) & 0xFF);
            Buffer[Pos + 2] = (byte)((Value >> 40) & 0xFF);
            Buffer[Pos + 1] = (byte)((Value >> 48) & 0xFF);
            Buffer[Pos] = (byte)((Value >> 56) & 0xFF);
        }
        #endregion

        #region Get/Set 8 bit word (S7 Byte) 16#00..16#FF
        public static byte GetByteAt(byte[] Buffer, int Pos)
        {
            return Buffer[Pos];
        }
        public static void SetByteAt(byte[] Buffer, int Pos, byte Value)
        {
            Buffer[Pos] = Value;
        }       
        #endregion

        #region Get/Set 16 bit word (S7 Word) 16#0000..16#FFFF
        public static UInt16 GetWordAt(byte[] Buffer, int Pos)
        {
            return GetUIntAt(Buffer,Pos);
        }
        public static void SetWordAt(byte[] Buffer, int Pos, UInt16 Value)
        {
            SetUIntAt(Buffer, Pos, Value);
        }
        #endregion

        #region Get/Set 32 bit word (S7 DWord) 16#00000000..16#FFFFFFFF
        public static UInt32 GetDWordAt(byte[] Buffer, int Pos)
        {
            return GetUDIntAt(Buffer, Pos);
        }
        public static void SetDWordAt(byte[] Buffer, int Pos, UInt32 Value)
        {
            SetUDIntAt(Buffer, Pos, Value);
        }
        #endregion

        #region Get/Set 64 bit word (S7 LWord) 16#0000000000000000..16#FFFFFFFFFFFFFFFF
        public static UInt64 GetLWordAt(byte[] Buffer, int Pos)
        {
            return GetULIntAt(Buffer, Pos);
        }
        public static void SetLWordAt(byte[] Buffer, int Pos, UInt64 Value)
        {
            SetULintAt(Buffer, Pos, Value);
        }
        #endregion

        #region Get/Set 32 bit floating point number (S7 Real) (Range of Single)
        public static Single GetRealAt(byte[] Buffer, int Pos)
        {
            UInt32 Value = GetUDIntAt(Buffer, Pos);
            byte[] bytes = BitConverter.GetBytes(Value);
            return BitConverter.ToSingle(bytes, 0);
        }
        public static void SetRealAt(byte[] Buffer, int Pos, Single Value)
        {
            byte[] FloatArray = BitConverter.GetBytes(Value);
            Buffer[Pos] = FloatArray[3];
            Buffer[Pos + 1] = FloatArray[2];
            Buffer[Pos + 2] = FloatArray[1];
            Buffer[Pos + 3] = FloatArray[0];
        }
        #endregion

        #region Get/Set 64 bit floating point number (S7 LReal) (Range of Double)
        public static Double GetLRealAt(byte[] Buffer, int Pos)
        {
            UInt64 Value = GetULIntAt(Buffer, Pos);
            byte[] bytes = BitConverter.GetBytes(Value);
            return BitConverter.ToDouble(bytes, 0);
        }
        public static void SetLRealAt(byte[] Buffer, int Pos, Double Value)
        {
            byte[] FloatArray = BitConverter.GetBytes(Value);
            Buffer[Pos] = FloatArray[7];
            Buffer[Pos + 1] = FloatArray[6];
            Buffer[Pos + 2] = FloatArray[5];
            Buffer[Pos + 3] = FloatArray[4];
            Buffer[Pos + 4] = FloatArray[3];
            Buffer[Pos + 5] = FloatArray[2];
            Buffer[Pos + 6] = FloatArray[1];
            Buffer[Pos + 7] = FloatArray[0];
        }
        #endregion

        #region Get/Set DateTime (S7 DATE_AND_TIME)
        public static DateTime GetDateTimeAt(byte[] Buffer, int Pos)
        {
            int Year, Month, Day, Hour, Min, Sec, MSec;

            Year = BCDtoByte(Buffer[Pos]);
            if (Year < 90)
                Year += 2000;
            else
                Year += 1900;

            Month = BCDtoByte(Buffer[Pos + 1]);
            Day = BCDtoByte(Buffer[Pos + 2]);
            Hour = BCDtoByte(Buffer[Pos + 3]);
            Min = BCDtoByte(Buffer[Pos + 4]);
            Sec = BCDtoByte(Buffer[Pos + 5]);
            MSec = (BCDtoByte(Buffer[Pos + 6]) * 10) + (BCDtoByte(Buffer[Pos + 7]) / 10);
            try
            {
                return new DateTime(Year, Month, Day, Hour, Min, Sec, MSec);
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }
        }
        public static void SetDateTimeAt(byte[] Buffer, int Pos, DateTime Value)
        {
            int Year = Value.Year;
            int Month = Value.Month;
            int Day = Value.Day;
            int Hour = Value.Hour;
            int Min = Value.Minute;
            int Sec = Value.Second;
            int Dow = (int)Value.DayOfWeek + 1;
            // MSecH = First two digits of miliseconds 
            int MsecH = Value.Millisecond / 10;
            // MSecL = Last digit of miliseconds
            int MsecL = Value.Millisecond % 10;
            if (Year > 1999)
                Year -= 2000;

            Buffer[Pos] = ByteToBCD(Year);
            Buffer[Pos + 1] = ByteToBCD(Month);
            Buffer[Pos + 2] = ByteToBCD(Day);
            Buffer[Pos + 3] = ByteToBCD(Hour);
            Buffer[Pos + 4] = ByteToBCD(Min);
            Buffer[Pos + 5] = ByteToBCD(Sec);
            Buffer[Pos + 6] = ByteToBCD(MsecH);
            Buffer[Pos + 7] = ByteToBCD(MsecL * 10 + Dow);
        }
        #endregion

        #region Get/Set DATE (S7 DATE) 
        public static DateTime GetDateAt(byte[] Buffer, int Pos)
        {
            try
            {
                return new DateTime(1990, 1, 1).AddDays(GetIntAt(Buffer, Pos));
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }
        }
        public static void SetDateAt(byte[] Buffer, int Pos, DateTime Value)
        {
            SetIntAt(Buffer, Pos, (Int16)(Value - new DateTime(1990, 1, 1)).Days);
        }

        #endregion

        #region Get/Set TOD (S7 TIME_OF_DAY)
        public static DateTime GetTODAt(byte[] Buffer, int Pos)
        {
            try
            {
                return new DateTime(0).AddMilliseconds(S7.GetDIntAt(Buffer, Pos));
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }
        }
        public static void SetTODAt(byte[] Buffer, int Pos, DateTime Value)
        {
            TimeSpan Time = Value.TimeOfDay;
            SetDIntAt(Buffer, Pos, (Int32)Math.Round(Time.TotalMilliseconds));          
        }
        #endregion
        
        #region Get/Set LTOD (S7 1500 LONG TIME_OF_DAY)
        public static DateTime GetLTODAt(byte[] Buffer, int Pos)
        {
            // .NET Tick = 100 ns, S71500 Tick = 1 ns
            try
            { 
                return new DateTime(Math.Abs(GetLIntAt(Buffer,Pos)/100));
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }
        }
        public static void SetLTODAt(byte[] Buffer, int Pos, DateTime Value)
        {
            TimeSpan Time = Value.TimeOfDay;
            SetLIntAt(Buffer, Pos, (Int64)Time.Ticks * 100);
        }
        #endregion

        #region GET/SET LDT (S7 1500 Long Date and Time)
        public static DateTime GetLDTAt(byte[] Buffer, int Pos)
        {
            try
            { 
                return new DateTime((GetLIntAt(Buffer, Pos) / 100) + bias); 
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }                 
        }
        public static void SetLDTAt(byte[] Buffer, int Pos, DateTime Value)
        {
            SetLIntAt(Buffer, Pos, (Value.Ticks-bias) * 100);
        }
        #endregion

        #region Get/Set DTL (S71200/1500 Date and Time)
        // Thanks to Johan Cardoen for GetDTLAt
        public static DateTime GetDTLAt(byte[] Buffer, int Pos)
        {
            int Year, Month, Day, Hour, Min, Sec, MSec;

            Year = Buffer[Pos] * 256 + Buffer[Pos + 1];
            Month = Buffer[Pos + 2];
            Day = Buffer[Pos + 3];
            Hour = Buffer[Pos + 5];
            Min = Buffer[Pos + 6];
            Sec = Buffer[Pos + 7];
            MSec = (int)GetUDIntAt(Buffer, Pos + 8)/1000000;

            try
            {
                return new DateTime(Year, Month, Day, Hour, Min, Sec, MSec);
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return new DateTime(0);
            }
        }
        public static void SetDTLAt(byte[] Buffer, int Pos, DateTime Value)
        {
            short Year = (short)Value.Year;
            byte Month = (byte)Value.Month;
            byte Day = (byte)Value.Day;
            byte Hour = (byte)Value.Hour;
            byte Min = (byte)Value.Minute;
            byte Sec = (byte)Value.Second;
            byte Dow = (byte)(Value.DayOfWeek + 1);

            Int32 NanoSecs = Value.Millisecond * 1000000;

            var bytes_short = BitConverter.GetBytes(Year);

            Buffer[Pos] = bytes_short[1];
            Buffer[Pos + 1] = bytes_short[0];
            Buffer[Pos + 2] = Month;
            Buffer[Pos + 3] = Day;
            Buffer[Pos + 4] = Dow;
            Buffer[Pos + 5] = Hour;
            Buffer[Pos + 6] = Min;
            Buffer[Pos + 7] = Sec;
            SetDIntAt(Buffer, Pos + 8, NanoSecs);
        }

        #endregion

        #region Get/Set String (S7 String)
        // Thanks to Pablo Agirre 
        public static string GetStringAt(byte[] Buffer, int Pos)
        {
            int size = (int)Buffer[Pos + 1];
            return Encoding.UTF8.GetString(Buffer, Pos + 2, size);
        }
        public static void SetStringAt(byte[] Buffer, int Pos, int MaxLen, string Value)
        {
            int size = Value.Length;
            Buffer[Pos] = (byte)MaxLen;
            Buffer[Pos + 1] = (byte)size;
            Encoding.UTF8.GetBytes(Value, 0, size, Buffer, Pos + 2);
        }
        #endregion

        #region Get/Set Array of char (S7 ARRAY OF CHARS)
        public static string GetCharsAt(byte[] Buffer, int Pos, int Size)
        {
            return Encoding.UTF8.GetString(Buffer, Pos , Size);
        }
        public static void SetCharsAt(byte[] Buffer, int Pos, string Value)
        {          
            int MaxLen = Buffer.Length - Pos;
            // Truncs the string if there's no room enough        
            if (MaxLen > Value.Length) MaxLen = Value.Length; 
            Encoding.UTF8.GetBytes(Value, 0, MaxLen, Buffer, Pos);
        }
        #endregion

        #region Get/Set Counter
        public static int GetCounter(ushort Value)
        {
            return BCDtoByte((byte)Value) * 100 + BCDtoByte((byte)(Value >> 8));
        }

        public static int GetCounterAt(ushort[] Buffer, int Index)
        {
            return GetCounter(Buffer[Index]);
        }

        public static ushort ToCounter(int Value)
        {
            return (ushort)(ByteToBCD(Value / 100) + (ByteToBCD(Value % 100) << 8));
        }

        public static void SetCounterAt(ushort[] Buffer, int Pos, int Value)
        {
            Buffer[Pos] = ToCounter(Value);
        }
        #endregion

        #endregion [Help Functions]
    }

    public class S7MultiVar
    {
        #region [MultiRead/Write Helper]
        private S7Client FClient;
        private GCHandle[] Handles = new GCHandle[S7Client.MaxVars];
        private int Count = 0;
        private S7Client.S7DataItem[] Items = new S7Client.S7DataItem[S7Client.MaxVars];


        public int[] Results = new int[S7Client.MaxVars];

        private bool AdjustWordLength(int Area, ref int WordLen, ref int Amount, ref int Start)
        {
            // Calc Word size          
            int WordSize = S7.DataSizeByte(WordLen);
            if (WordSize == 0)
                return false;

            if (Area == S7Consts.S7AreaCT)
                WordLen = S7Consts.S7WLCounter;
            if (Area == S7Consts.S7AreaTM)
                WordLen = S7Consts.S7WLTimer;

            if (WordLen == S7Consts.S7WLBit)
                Amount = 1;  // Only 1 bit can be transferred at time
            else
            {
                if ((WordLen != S7Consts.S7WLCounter) && (WordLen != S7Consts.S7WLTimer))
                {
                    Amount = Amount * WordSize;
                    Start = Start * 8;
                    WordLen = S7Consts.S7WLByte;
                }
            }   
            return true;
        }

        public S7MultiVar(S7Client Client)
        {
            FClient = Client;
            for (int c = 0; c < S7Client.MaxVars; c++)
                Results[c] = (int)S7Consts.errCliItemNotAvailable;
        }
        ~S7MultiVar()
        {
            Clear();
        }

        public bool Add<T>(S7Consts.S7Tag Tag, ref T[] Buffer, int Offset)
        {
            return Add(Tag.Area, Tag.WordLen, Tag.DBNumber, Tag.Start, Tag.Elements, ref Buffer, Offset);
        }

        public bool Add<T>(S7Consts.S7Tag Tag, ref T[] Buffer)
        {
            return Add(Tag.Area, Tag.WordLen, Tag.DBNumber, Tag.Start, Tag.Elements, ref Buffer);
        }

        public bool Add<T>(Int32 Area, Int32 WordLen, Int32 DBNumber, Int32 Start, Int32 Amount, ref T[] Buffer)
        {
            return Add(Area, WordLen, DBNumber, Start, Amount, ref Buffer, 0);
        }

        public bool Add<T>(Int32 Area, Int32 WordLen, Int32 DBNumber, Int32 Start, Int32 Amount, ref T[] Buffer, int Offset)
        {
            if (Count < S7Client.MaxVars)
            {
                if (AdjustWordLength(Area, ref WordLen, ref Amount, ref Start))
                {
                    Items[Count].Area = Area;
                    Items[Count].WordLen = WordLen;
                    Items[Count].Result = (int)S7Consts.errCliItemNotAvailable;
                    Items[Count].DBNumber = DBNumber;
                    Items[Count].Start = Start;
                    Items[Count].Amount = Amount;
                    GCHandle handle = GCHandle.Alloc(Buffer, GCHandleType.Pinned);
#if WINDOWS_UWP || NETFX_CORE
                    if (IntPtr.Size == 4)
                        Items[Count].pData = (IntPtr)(handle.AddrOfPinnedObject().ToInt32() + Offset * Marshal.SizeOf<T>());
                    else
                        Items[Count].pData = (IntPtr)(handle.AddrOfPinnedObject().ToInt64() + Offset * Marshal.SizeOf<T>());
#else
                    if (IntPtr.Size == 4)
                        Items[Count].pData = (IntPtr)(handle.AddrOfPinnedObject().ToInt32() + Offset * Marshal.SizeOf(typeof(T)));
                    else
                        Items[Count].pData = (IntPtr)(handle.AddrOfPinnedObject().ToInt64() + Offset * Marshal.SizeOf(typeof(T)));
#endif
                    Handles[Count] = handle;
                    Count++;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }

        public int Read()
        {
            int FunctionResult;
            int GlobalResult = (int)S7Consts.errCliFunctionRefused;
            try
            {
                if (Count > 0)
                {
                    FunctionResult = FClient.ReadMultiVars(Items, Count);
                    if (FunctionResult == 0)
                        for (int c = 0; c < S7Client.MaxVars; c++)
                            Results[c] = Items[c].Result;
                    GlobalResult = FunctionResult;
                }
                else
                    GlobalResult = (int)S7Consts.errCliFunctionRefused;
            }
            finally
            {
                Clear(); // handles are no more needed and MUST be freed
            }
            return GlobalResult;
        }

        public int Write()
        {
            int FunctionResult;
            int GlobalResult = (int)S7Consts.errCliFunctionRefused;
            try
            {
                if (Count > 0)
                {
                    FunctionResult = FClient.WriteMultiVars(Items, Count);
                    if (FunctionResult == 0)
                        for (int c = 0; c < S7Client.MaxVars; c++)
                            Results[c] = Items[c].Result;
                    GlobalResult = FunctionResult;
                }
                else
                    GlobalResult = (int)S7Consts.errCliFunctionRefused;
            }
            finally
            {
                Clear(); // handles are no more needed and MUST be freed
            }
            return GlobalResult;
        }

        public void Clear()
        {
            for (int c = 0; c < Count; c++)
            {
                if (Handles[c] != null)
                    Handles[c].Free();
            }
            Count = 0;
        }
        #endregion
    }

    public class S7Client
    {
        #region [Constants and TypeDefs]

        // Block type
        public const int Block_OB  = 0x38;
        public const int Block_DB  = 0x41;
        public const int Block_SDB = 0x42;
        public const int Block_FC  = 0x43;
        public const int Block_SFC = 0x44;
        public const int Block_FB  = 0x45;
        public const int Block_SFB = 0x46;

        // Sub Block Type 
        public const byte SubBlk_OB  = 0x08;
        public const byte SubBlk_DB  = 0x0A;
        public const byte SubBlk_SDB = 0x0B;
        public const byte SubBlk_FC  = 0x0C;
        public const byte SubBlk_SFC = 0x0D;
        public const byte SubBlk_FB  = 0x0E;
        public const byte SubBlk_SFB = 0x0F;

        // Block languages
        public const byte BlockLangAWL   = 0x01;
        public const byte BlockLangKOP   = 0x02;
        public const byte BlockLangFUP   = 0x03;
        public const byte BlockLangSCL   = 0x04;
        public const byte BlockLangDB    = 0x05;
        public const byte BlockLangGRAPH = 0x06;

        // Max number of vars (multiread/write)
        public static readonly int MaxVars = 20;
        
        // Result transport size
        const byte TS_ResBit   = 0x03;
        const byte TS_ResByte  = 0x04;
        const byte TS_ResInt   = 0x05;
        const byte TS_ResReal  = 0x07;
        const byte TS_ResOctet = 0x09;

        const ushort Code7Ok                    = 0x0000;
        const ushort Code7AddressOutOfRange     = 0x0005;
        const ushort Code7InvalidTransportSize  = 0x0006;
        const ushort Code7WriteDataSizeMismatch = 0x0007;
        const ushort Code7ResItemNotAvailable   = 0x000A;
        const ushort Code7ResItemNotAvailable1  = 0xD209;
        const ushort Code7InvalidValue          = 0xDC01;
        const ushort Code7NeedPassword          = 0xD241;
        const ushort Code7InvalidPassword       = 0xD602;
        const ushort Code7NoPasswordToClear     = 0xD604;
        const ushort Code7NoPasswordToSet       = 0xD605;
        const ushort Code7FunNotAvailable       = 0x8104;
        const ushort Code7DataOverPDU           = 0x8500;

        // Client Connection Type
        public static readonly UInt16 CONNTYPE_PG    = 0x01;  // Connect to the PLC as a PG
        public static readonly UInt16 CONNTYPE_OP    = 0x02;  // Connect to the PLC as an OP
        public static readonly UInt16 CONNTYPE_BASIC = 0x03;  // Basic connection 

        public int _LastError = 0;

        public struct S7DataItem
        {
            public int Area;
            public int WordLen;
            public int Result;
            public int DBNumber;
            public int Start;
            public int Amount;
            public IntPtr pData;
        }

        // Order Code + Version
        public struct S7OrderCode
        {
            public string Code; // such as "6ES7 151-8AB01-0AB0"
            public byte V1;     // Version 1st digit
            public byte V2;     // Version 2nd digit
            public byte V3;     // Version 3th digit
        };

        // CPU Info
        public struct S7CpuInfo
        {
            public string ModuleTypeName;
            public string SerialNumber;
            public string ASName;
            public string Copyright;
            public string ModuleName;
        }

        public struct S7CpInfo
        {
            public int MaxPduLength;
            public int MaxConnections;
            public int MaxMpiRate;
            public int MaxBusRate;
        };

        // Block List
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct S7BlocksList
        {
            public Int32 OBCount;
            public Int32 FBCount;
            public Int32 FCCount;
            public Int32 SFBCount;
            public Int32 SFCCount;
            public Int32 DBCount;
            public Int32 SDBCount;
        };

        // Managed Block Info
        public struct S7BlockInfo
        {
            public int BlkType;
            public int BlkNumber;
            public int BlkLang;
            public int BlkFlags;
            public int MC7Size;  // The real size in bytes
            public int LoadSize;
            public int LocalData;
            public int SBBLength;
            public int CheckSum;
            public int Version;
            // Chars info
            public string CodeDate;
            public string IntfDate;
            public string Author;
            public string Family;
            public string Header;
        };

        // See §33.1 of "System Software for S7-300/400 System and Standard Functions"
        // and see SFC51 description too
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct SZL_HEADER
        {
            public UInt16 LENTHDR;
            public UInt16 N_DR;
        };

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct S7SZL
        {
            public SZL_HEADER Header;
            [MarshalAs(UnmanagedType.ByValArray)]
            public byte[] Data;
        };

        // SZL List of available SZL IDs : same as SZL but List items are big-endian adjusted
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct S7SZLList
        {
            public SZL_HEADER Header;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 0x2000 - 2)]
            public UInt16[] Data;
        };

        // S7 Protection
        // See §33.19 of "System Software for S7-300/400 System and Standard Functions"
        public struct S7Protection  
        {
            public ushort sch_schal;
            public ushort sch_par;
            public ushort sch_rel;
            public ushort bart_sch;
            public ushort anl_sch;
        };

        #endregion

        #region [S7 Telegrams]

        // ISO Connection Request telegram (contains also ISO Header and COTP Header)
        byte[] ISO_CR = {
            // TPKT (RFC1006 Header)
            0x03, // RFC 1006 ID (3) 
            0x00, // Reserved, always 0
            0x00, // High part of packet lenght (entire frame, payload and TPDU included)
            0x16, // Low part of packet lenght (entire frame, payload and TPDU included)
            // COTP (ISO 8073 Header)
            0x11, // PDU Size Length
            0xE0, // CR - Connection Request ID
            0x00, // Dst Reference HI
            0x00, // Dst Reference LO
            0x00, // Src Reference HI
            0x01, // Src Reference LO
            0x00, // Class + Options Flags
            0xC0, // PDU Max Length ID
            0x01, // PDU Max Length HI
            0x0A, // PDU Max Length LO
            0xC1, // Src TSAP Identifier
            0x02, // Src TSAP Length (2 bytes)
            0x01, // Src TSAP HI (will be overwritten)
            0x00, // Src TSAP LO (will be overwritten)
            0xC2, // Dst TSAP Identifier
            0x02, // Dst TSAP Length (2 bytes)
            0x01, // Dst TSAP HI (will be overwritten)
            0x02  // Dst TSAP LO (will be overwritten)
        };

        // TPKT + ISO COTP Header (Connection Oriented Transport Protocol)
        byte[] TPKT_ISO = { // 7 bytes
            0x03,0x00,
            0x00,0x1f,      // Telegram Length (Data Size + 31 or 35)
            0x02,0xf0,0x80  // COTP (see above for info)
        };

        // S7 PDU Negotiation Telegram (contains also ISO Header and COTP Header)
        byte[] S7_PN = {
            0x03, 0x00, 0x00, 0x19, 
            0x02, 0xf0, 0x80, // TPKT + COTP (see above for info)
            0x32, 0x01, 0x00, 0x00, 
            0x04, 0x00, 0x00, 0x08, 
            0x00, 0x00, 0xf0, 0x00, 
            0x00, 0x01, 0x00, 0x01, 
            0x00, 0x1e        // PDU Length Requested = HI-LO Here Default 480 bytes
        };
        
        // S7 Read/Write Request Header (contains also ISO Header and COTP Header)
        byte[] S7_RW = { // 31-35 bytes
            0x03,0x00, 
            0x00,0x1f,       // Telegram Length (Data Size + 31 or 35)
            0x02,0xf0, 0x80, // COTP (see above for info)
            0x32,            // S7 Protocol ID 
            0x01,            // Job Type
            0x00,0x00,       // Redundancy identification
            0x05,0x00,       // PDU Reference
            0x00,0x0e,       // Parameters Length
            0x00,0x00,       // Data Length = Size(bytes) + 4      
            0x04,            // Function 4 Read Var, 5 Write Var  
            0x01,            // Items count
            0x12,            // Var spec.
            0x0a,            // Length of remaining bytes
            0x10,            // Syntax ID 
            (byte)S7Consts.S7WLByte,  // Transport Size idx=22                       
            0x00,0x00,       // Num Elements                          
            0x00,0x00,       // DB Number (if any, else 0)            
            0x84,            // Area Type                            
            0x00,0x00,0x00,  // Area Offset                     
            // WR area
            0x00,            // Reserved 
            0x04,            // Transport size
            0x00,0x00,       // Data Length * 8 (if not bit or timer or counter) 
        };
        private static int Size_RD = 31; // Header Size when Reading 
        private static int Size_WR = 35; // Header Size when Writing

        // S7 Variable MultiRead Header
        byte[] S7_MRD_HEADER = {
            0x03,0x00, 
            0x00,0x1f,       // Telegram Length 
            0x02,0xf0, 0x80, // COTP (see above for info)
            0x32,            // S7 Protocol ID 
            0x01,            // Job Type
            0x00,0x00,       // Redundancy identification
            0x05,0x00,       // PDU Reference
            0x00,0x0e,       // Parameters Length
            0x00,0x00,       // Data Length = Size(bytes) + 4      
            0x04,            // Function 4 Read Var, 5 Write Var  
            0x01             // Items count (idx 18)
        };

        // S7 Variable MultiRead Item
        byte[] S7_MRD_ITEM = {
            0x12,            // Var spec.
            0x0a,            // Length of remaining bytes
            0x10,            // Syntax ID 
            (byte)S7Consts.S7WLByte,  // Transport Size idx=3                   
            0x00,0x00,       // Num Elements                          
            0x00,0x00,       // DB Number (if any, else 0)            
            0x84,            // Area Type                            
            0x00,0x00,0x00   // Area Offset                     
        };

        // S7 Variable MultiWrite Header
        byte[] S7_MWR_HEADER = {
            0x03,0x00,
            0x00,0x1f,       // Telegram Length 
            0x02,0xf0, 0x80, // COTP (see above for info)
            0x32,            // S7 Protocol ID 
            0x01,            // Job Type
            0x00,0x00,       // Redundancy identification
            0x05,0x00,       // PDU Reference
            0x00,0x0e,       // Parameters Length (idx 13)
            0x00,0x00,       // Data Length = Size(bytes) + 4 (idx 15)     
            0x05,            // Function 5 Write Var  
            0x01             // Items count (idx 18)
        };

        // S7 Variable MultiWrite Item (Param)
        byte[] S7_MWR_PARAM = {
            0x12,            // Var spec.
            0x0a,            // Length of remaining bytes
            0x10,            // Syntax ID 
            (byte)S7Consts.S7WLByte,  // Transport Size idx=3                      
            0x00,0x00,       // Num Elements                          
            0x00,0x00,       // DB Number (if any, else 0)            
            0x84,            // Area Type                            
            0x00,0x00,0x00,  // Area Offset                     
        };

        // SZL First telegram request   
        byte[] S7_SZL_FIRST = {
            0x03, 0x00, 0x00, 0x21,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00,
            0x05, 0x00, // Sequence out
            0x00, 0x08, 0x00,
            0x08, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x44, 0x01,
            0x00, 0xff, 0x09, 0x00,
            0x04,
            0x00, 0x00, // ID (29)
            0x00, 0x00  // Index (31)
        };

        // SZL Next telegram request 
        byte[] S7_SZL_NEXT = {
            0x03, 0x00, 0x00, 0x21,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x06,
            0x00, 0x00, 0x0c, 0x00,
            0x04, 0x00, 0x01, 0x12,
            0x08, 0x12, 0x44, 0x01,
            0x01, // Sequence
            0x00, 0x00, 0x00, 0x00,
            0x0a, 0x00, 0x00, 0x00
        };

        // Get Date/Time request
        byte[] S7_GET_DT = {
            0x03, 0x00, 0x00, 0x1d,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x38,
            0x00, 0x00, 0x08, 0x00,
            0x04, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x47, 0x01,
            0x00, 0x0a, 0x00, 0x00,
            0x00
        };

        // Set Date/Time command
        byte[] S7_SET_DT = {
            0x03, 0x00, 0x00, 0x27,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x89,
            0x03, 0x00, 0x08, 0x00,
            0x0e, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x47, 0x02,
            0x00, 0xff, 0x09, 0x00,
            0x0a, 0x00,
            0x19, // Hi part of Year (idx=30)
            0x13, // Lo part of Year
            0x12, // Month
            0x06, // Day
            0x17, // Hour
            0x37, // Min
            0x13, // Sec
            0x00, 0x01 // ms + Day of week   
        };

        // S7 Set Session Password 
        byte[] S7_SET_PWD = {
            0x03, 0x00, 0x00, 0x25,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x27,
            0x00, 0x00, 0x08, 0x00,
            0x0c, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x45, 0x01,
            0x00, 0xff, 0x09, 0x00,
            0x08, 
            // 8 Char Encoded Password
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };

        // S7 Clear Session Password 
        byte[] S7_CLR_PWD = {
            0x03, 0x00, 0x00, 0x1d,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x29,
            0x00, 0x00, 0x08, 0x00,
            0x04, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x45, 0x02,
            0x00, 0x0a, 0x00, 0x00,
            0x00
        };

        // S7 STOP request
        byte[] S7_STOP = {
            0x03, 0x00, 0x00, 0x21,
            0x02, 0xf0, 0x80, 0x32,
            0x01, 0x00, 0x00, 0x0e,
            0x00, 0x00, 0x10, 0x00,
            0x00, 0x29, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x09,
            0x50, 0x5f, 0x50, 0x52,
            0x4f, 0x47, 0x52, 0x41,
            0x4d
        };

        // S7 HOT Start request
        byte[] S7_HOT_START = {
            0x03, 0x00, 0x00, 0x25,
            0x02, 0xf0, 0x80, 0x32,
            0x01, 0x00, 0x00, 0x0c,
            0x00, 0x00, 0x14, 0x00,
            0x00, 0x28, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0xfd, 0x00, 0x00, 0x09,
            0x50, 0x5f, 0x50, 0x52,
            0x4f, 0x47, 0x52, 0x41,
            0x4d
        };

        // S7 COLD Start request
        byte[] S7_COLD_START = {
            0x03, 0x00, 0x00, 0x27,
            0x02, 0xf0, 0x80, 0x32,
            0x01, 0x00, 0x00, 0x0f,
            0x00, 0x00, 0x16, 0x00,
            0x00, 0x28, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0xfd, 0x00, 0x02, 0x43,
            0x20, 0x09, 0x50, 0x5f,
            0x50, 0x52, 0x4f, 0x47,
            0x52, 0x41, 0x4d
        };
        const byte pduStart          = 0x28;   // CPU start
        const byte pduStop           = 0x29;   // CPU stop
        const byte pduAlreadyStarted = 0x02;   // CPU already in run mode
        const byte pduAlreadyStopped = 0x07;   // CPU already in stop mode

        // S7 Get PLC Status 
        byte[] S7_GET_STAT = {
            0x03, 0x00, 0x00, 0x21,
            0x02, 0xf0, 0x80, 0x32,
            0x07, 0x00, 0x00, 0x2c,
            0x00, 0x00, 0x08, 0x00,
            0x08, 0x00, 0x01, 0x12,
            0x04, 0x11, 0x44, 0x01,
            0x00, 0xff, 0x09, 0x00,
            0x04, 0x04, 0x24, 0x00,
            0x00
        };

        // S7 Get Block Info Request Header (contains also ISO Header and COTP Header)
        byte[] S7_BI = {
            0x03, 0x00, 0x00, 0x25, 
            0x02, 0xf0, 0x80, 0x32, 
            0x07, 0x00, 0x00, 0x05, 
            0x00, 0x00, 0x08, 0x00, 
            0x0c, 0x00, 0x01, 0x12, 
            0x04, 0x11, 0x43, 0x03, 
            0x00, 0xff, 0x09, 0x00, 
            0x08, 0x30, 
            0x41, // Block Type
            0x30, 0x30, 0x30, 0x30, 0x30, // ASCII Block Number
            0x41 
	    };    

        #endregion

        #region [Internals]

        // Defaults
        private static int ISOTCP = 102; // ISOTCP Port
        private static int MinPduSize = 16;
        private static int MinPduSizeToRequest = 240;
        private static int MaxPduSizeToRequest = 960;
        private static int DefaultTimeout = 2000;
        private static int IsoHSize = 7; // TPKT+COTP Header Size

        // Properties
        private int _PDULength = 0;
        private int _PduSizeRequested = 480;
        private int _PLCPort = ISOTCP;
        private int _RecvTimeout = DefaultTimeout;
        private int _SendTimeout = DefaultTimeout;
        private int _ConnTimeout = DefaultTimeout;

        // Privates
        private string IPAddress;
        private byte LocalTSAP_HI;
        private byte LocalTSAP_LO;
        private byte RemoteTSAP_HI;
        private byte RemoteTSAP_LO;
        private byte LastPDUType;
        private ushort ConnType = CONNTYPE_PG;
        private byte[] PDU = new byte[2048];
        private MsgSocket Socket = null;
        private int Time_ms = 0;

        private void CreateSocket()
        {
            try
            {
                Socket = new MsgSocket(); 
                Socket.ConnectTimeout = _ConnTimeout;
                Socket.ReadTimeout = _RecvTimeout;
                Socket.WriteTimeout = _SendTimeout;
            }
            catch
            {
            }
        }

        private int TCPConnect()
        {           
            if (_LastError==0)
            try
            {
                _LastError=Socket.Connect(IPAddress, _PLCPort);
            }
            catch
            {
                _LastError = S7Consts.errTCPConnectionFailed;
            }
            return _LastError;
        }
       
        private void RecvPacket(byte[] Buffer, int Start, int Size)
        {
            if (Connected)
                _LastError = Socket.Receive(Buffer, Start, Size);
            else
                _LastError = S7Consts.errTCPNotConnected;
        }

        private void SendPacket(byte[] Buffer, int Len)
        {
            _LastError = Socket.Send(Buffer, Len);
        }

        private void SendPacket(byte[] Buffer)
        {
            if (Connected)
                SendPacket(Buffer, Buffer.Length);
            else
                _LastError = S7Consts.errTCPNotConnected;
        }

        private int RecvIsoPacket()
        {
            Boolean Done = false;
            int Size = 0;
            while ((_LastError == 0) && !Done)
            {
                // Get TPKT (4 bytes)
                RecvPacket(PDU, 0, 4);
                if (_LastError == 0)
                {
                    Size = S7.GetWordAt(PDU, 2);
                    // Check 0 bytes Data Packet (only TPKT+COTP = 7 bytes)
                    if (Size == IsoHSize)
                        RecvPacket(PDU, 4, 3); // Skip remaining 3 bytes and Done is still false
                    else
                    {
                        if ((Size > _PduSizeRequested + IsoHSize) || (Size < MinPduSize))
                            _LastError = S7Consts.errIsoInvalidPDU;
                        else
                            Done = true; // a valid Length !=7 && >16 && <247
                    }
                }
            }
            if (_LastError == 0)
            {
                RecvPacket(PDU, 4, 3); // Skip remaining 3 COTP bytes
                LastPDUType = PDU[5];   // Stores PDU Type, we need it 
                // Receives the S7 Payload          
                RecvPacket(PDU, 7, Size - IsoHSize);
            }
            if (_LastError == 0)
                return Size;
            else
                return 0;
        }

        private int ISOConnect()
        {
            int Size;
            ISO_CR[16] = LocalTSAP_HI;
            ISO_CR[17] = LocalTSAP_LO;
            ISO_CR[20] = RemoteTSAP_HI;
            ISO_CR[21] = RemoteTSAP_LO;

            // Sends the connection request telegram      
            SendPacket(ISO_CR);
            if (_LastError == 0)
            {
                // Gets the reply (if any)
                Size = RecvIsoPacket();
                if (_LastError == 0)
                {
                    if (Size == 22)
                    {
                        if (LastPDUType != (byte)0xD0) // 0xD0 = CC Connection confirm
                            _LastError = S7Consts.errIsoConnect;
                    }
                    else
                        _LastError = S7Consts.errIsoInvalidPDU;
                }
            }
            return _LastError;
        }

        private int NegotiatePduLength()
        {
            int Length;
            // Set PDU Size Requested
            S7.SetWordAt(S7_PN, 23, (ushort)_PduSizeRequested);
            // Sends the connection request telegram
            SendPacket(S7_PN);
            if (_LastError == 0)
            {
                Length = RecvIsoPacket();
                if (_LastError == 0)
                {
                    // check S7 Error
                    if ((Length == 27) && (PDU[17] == 0) && (PDU[18] == 0))  // 20 = size of Negotiate Answer
                    {
                        // Get PDU Size Negotiated
                        _PDULength = S7.GetWordAt(PDU, 25);
                        if (_PDULength <= 0)
                            _LastError = S7Consts.errCliNegotiatingPDU;
                    }
                    else
                        _LastError = S7Consts.errCliNegotiatingPDU;
                }
            }
            return _LastError;
        }

        private int CpuError(ushort Error)
        {
          switch(Error)
          {
            case 0                          : return 0;
            case Code7AddressOutOfRange     : return S7Consts.errCliAddressOutOfRange;
            case Code7InvalidTransportSize  : return S7Consts.errCliInvalidTransportSize;
            case Code7WriteDataSizeMismatch : return S7Consts.errCliWriteDataSizeMismatch;
            case Code7ResItemNotAvailable   :
            case Code7ResItemNotAvailable1  : return S7Consts.errCliItemNotAvailable;
            case Code7DataOverPDU           : return S7Consts.errCliSizeOverPDU;
            case Code7InvalidValue          : return S7Consts.errCliInvalidValue;
            case Code7FunNotAvailable       : return S7Consts.errCliFunNotAvailable;
            case Code7NeedPassword          : return S7Consts.errCliNeedPassword;
            case Code7InvalidPassword       : return S7Consts.errCliInvalidPassword;
            case Code7NoPasswordToSet       :
            case Code7NoPasswordToClear     : return S7Consts.errCliNoPasswordToSetOrClear;
          default:
            return S7Consts.errCliFunctionRefused;
          };
        }

        #endregion

        #region [Class Control]

        public S7Client()
        {
            CreateSocket();
        }

        ~S7Client()
        {
            Disconnect();
        }

        public int Connect()
        {
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;
            if (!Connected)
            {
                TCPConnect(); // First stage : TCP Connection
                if (_LastError == 0) 
                {
                    ISOConnect(); // Second stage : ISOTCP (ISO 8073) Connection
                    if (_LastError == 0) 
                    {
                        _LastError = NegotiatePduLength(); // Third stage : S7 PDU negotiation
                    }
                }
            }
            if (_LastError != 0)
                Disconnect();
            else
                Time_ms = Environment.TickCount - Elapsed;

            return _LastError;
        }

        public int ConnectTo(string Address, int Rack, int Slot)
        {
            UInt16 RemoteTSAP = (UInt16)((ConnType << 8) + (Rack * 0x20) + Slot);
            SetConnectionParams(Address, 0x0100, RemoteTSAP);
            return Connect();
        }

        public int SetConnectionParams(string Address, ushort LocalTSAP, ushort RemoteTSAP)
        {
            int LocTSAP = LocalTSAP & 0x0000FFFF;
            int RemTSAP = RemoteTSAP & 0x0000FFFF;
            IPAddress = Address;
            LocalTSAP_HI = (byte)(LocTSAP >> 8);
            LocalTSAP_LO = (byte)(LocTSAP & 0x00FF);
            RemoteTSAP_HI = (byte)(RemTSAP >> 8);
            RemoteTSAP_LO = (byte)(RemTSAP & 0x00FF);
            return 0;
        }

        public int SetConnectionType(ushort ConnectionType)
        {
            ConnType = ConnectionType;
            return 0;
        }

        public int Disconnect()
        {
            Socket.Close();
            return 0;
        }

        public int GetParam(Int32 ParamNumber, ref int Value)
        {
            int Result = 0;
            switch (ParamNumber)
            {
                case S7Consts.p_u16_RemotePort:
                    {
                        Value = PLCPort;
                        break;
                    }
                case S7Consts.p_i32_PingTimeout:
                    {
                        Value = ConnTimeout;
                        break;
                    }
                case S7Consts.p_i32_SendTimeout:
                    {
                        Value = SendTimeout;
                        break;
                    }
                case S7Consts.p_i32_RecvTimeout:
                    {
                        Value = RecvTimeout;
                        break;
                    }
                case S7Consts.p_i32_PDURequest:
                    {
                        Value = PduSizeRequested;
                        break;
                    }
                default:
                    {
                        Result = S7Consts.errCliInvalidParamNumber;
                        break;
                    }
            }
            return Result;
        }

        // Set Properties for compatibility with Snap7.net.cs
        public int SetParam(Int32 ParamNumber, ref int Value)
        {
            int Result = 0;
            switch(ParamNumber)
            {
                case S7Consts.p_u16_RemotePort:
                    {
                        PLCPort = Value;
                        break;
                    }
                case S7Consts.p_i32_PingTimeout:
                    {
                        ConnTimeout = Value;
                        break;
                    }
                case S7Consts.p_i32_SendTimeout:
                    {
                        SendTimeout = Value;
                        break;
                    }
                case S7Consts.p_i32_RecvTimeout:
                    {
                        RecvTimeout = Value;
                        break;
                    }
                case S7Consts.p_i32_PDURequest:
                    {
                        PduSizeRequested = Value;
                        break;
                    }
                default:
                    {
                        Result = S7Consts.errCliInvalidParamNumber;
                        break;
                    }
            }
            return Result; 
        }

        public delegate void S7CliCompletion(IntPtr usrPtr, int opCode, int opResult);
        public int SetAsCallBack(S7CliCompletion Completion, IntPtr usrPtr)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        #endregion

        #region [Data I/O main functions]

        public int ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer)
        {
            int BytesRead = 0;
            return ReadArea(Area, DBNumber, Start, Amount, WordLen, Buffer, ref BytesRead);
        }

        public int ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer, ref int BytesRead)
        {
            int Address;
            int NumElements;
            int MaxElements;
            int TotElements;
            int SizeRequested;
            int Length;
            int Offset = 0;
            int WordSize = 1;

            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;
            // Some adjustment
            if (Area == S7Consts.S7AreaCT)
                WordLen = S7Consts.S7WLCounter;
            if (Area == S7Consts.S7AreaTM)
                WordLen = S7Consts.S7WLTimer;

            // Calc Word size          
            WordSize = S7.DataSizeByte(WordLen);
            if (WordSize == 0)
                return S7Consts.errCliInvalidWordLen;

            if (WordLen == S7Consts.S7WLBit)
                Amount = 1;  // Only 1 bit can be transferred at time
            else
            {
                if ((WordLen != S7Consts.S7WLCounter) && (WordLen != S7Consts.S7WLTimer))
                {
                    Amount = Amount * WordSize;
                    WordSize = 1;
                    WordLen = S7Consts.S7WLByte;
                }
            }        

            MaxElements = (_PDULength - 18) / WordSize; // 18 = Reply telegram header
            TotElements = Amount;

            while ((TotElements > 0) && (_LastError == 0))
            {
                NumElements = TotElements;
                if (NumElements > MaxElements)
                    NumElements = MaxElements;

                SizeRequested = NumElements * WordSize;

                // Setup the telegram
                Array.Copy(S7_RW, 0, PDU, 0, Size_RD);
                // Set DB Number
                PDU[27] = (byte)Area;
                // Set Area
                if (Area == S7Consts.S7AreaDB)
                    S7.SetWordAt(PDU, 25, (ushort)DBNumber);

                // Adjusts Start and word length
                if ((WordLen == S7Consts.S7WLBit) || (WordLen == S7Consts.S7WLCounter) || (WordLen == S7Consts.S7WLTimer))
                {
                    Address = Start;
                    PDU[22] = (byte)WordLen;
                }
                else
                    Address = Start << 3;

                // Num elements
                S7.SetWordAt(PDU, 23, (ushort)NumElements);

                // Address into the PLC (only 3 bytes)           
                PDU[30] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                PDU[29] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                PDU[28] = (byte)(Address & 0x0FF);

                SendPacket(PDU, Size_RD);
                if (_LastError == 0)
                {
                    Length = RecvIsoPacket();
                    if (_LastError == 0)
                    {
                        if (Length<25)
                            _LastError = S7Consts.errIsoInvalidDataSize;
                        else
                        {
                            if (PDU[21] != 0xFF)
                                _LastError = CpuError(PDU[21]);
                            else
                            {
                                Array.Copy(PDU, 25, Buffer, Offset, SizeRequested);
                                Offset += SizeRequested;
                            }
                        }
                    }
                }
                TotElements -= NumElements;
                Start += NumElements * WordSize;
            }

            if (_LastError == 0)
            {
                BytesRead = Offset;
                Time_ms = Environment.TickCount - Elapsed;
            }
            else
                BytesRead = 0;
            return _LastError;
        }

        public int WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer)
        {
            int BytesWritten = 0;
            return WriteArea(Area, DBNumber, Start, Amount, WordLen, Buffer, ref BytesWritten);
        }

        public int WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer, ref int BytesWritten)
        {
            int Address;
            int NumElements;
            int MaxElements;
            int TotElements;
            int DataSize;
            int IsoSize;
            int Length;
            int Offset = 0;
            int WordSize = 1;

            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;
            // Some adjustment
            if (Area == S7Consts.S7AreaCT)
                WordLen = S7Consts.S7WLCounter;
            if (Area == S7Consts.S7AreaTM)
                WordLen = S7Consts.S7WLTimer;

            // Calc Word size          
            WordSize = S7.DataSizeByte(WordLen);
            if (WordSize == 0)
                return S7Consts.errCliInvalidWordLen;

            if (WordLen == S7Consts.S7WLBit) // Only 1 bit can be transferred at time
                Amount = 1;
            else
            {
                if ((WordLen != S7Consts.S7WLCounter) && (WordLen != S7Consts.S7WLTimer))
                {
                    Amount = Amount * WordSize;
                    WordSize = 1;
                    WordLen = S7Consts.S7WLByte;
                }
            }        

            MaxElements = (_PDULength - 35) / WordSize; // 35 = Reply telegram header
            TotElements = Amount;

            while ((TotElements > 0) && (_LastError == 0))
            {
                NumElements = TotElements;
                if (NumElements > MaxElements)
                    NumElements = MaxElements;

                DataSize = NumElements * WordSize;
                IsoSize = Size_WR + DataSize;

                // Setup the telegram
                Array.Copy(S7_RW, 0, PDU, 0, Size_WR);
                // Whole telegram Size
                S7.SetWordAt(PDU, 2, (ushort)IsoSize);
                // Data Length
                Length = DataSize + 4;
                S7.SetWordAt(PDU, 15, (ushort)Length);
                // Function
                PDU[17] = (byte)0x05;
                // Set DB Number
                PDU[27] = (byte)Area;
                if (Area == S7Consts.S7AreaDB)
                    S7.SetWordAt(PDU, 25, (ushort)DBNumber);


                // Adjusts Start and word length
                if ((WordLen == S7Consts.S7WLBit) || (WordLen == S7Consts.S7WLCounter) || (WordLen == S7Consts.S7WLTimer))
                {
                    Address = Start;
                    Length = DataSize;
                    PDU[22] = (byte)WordLen;
                }
                else
                {
                    Address = Start << 3;
                    Length = DataSize << 3;
                }

                // Num elements
                S7.SetWordAt(PDU, 23, (ushort)NumElements);
                // Address into the PLC
                PDU[30] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                PDU[29] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                PDU[28] = (byte)(Address & 0x0FF);

                // Transport Size
                switch (WordLen)
                {
                    case S7Consts.S7WLBit:
                        PDU[32] = TS_ResBit;
                        break;
                    case S7Consts.S7WLCounter:
                    case S7Consts.S7WLTimer:
                        PDU[32] = TS_ResOctet;
                        break;
                    default:
                        PDU[32] = TS_ResByte; // byte/word/dword etc.
                        break;
                };
                // Length
                S7.SetWordAt(PDU, 33, (ushort)Length);

                // Copies the Data
                Array.Copy(Buffer, Offset, PDU, 35, DataSize);

                SendPacket(PDU, IsoSize);
                if (_LastError == 0)
                {
                    Length = RecvIsoPacket();
                    if (_LastError == 0)
                    {
                        if (Length == 22)
                        {
                            if (PDU[21] != (byte)0xFF)
                                _LastError = CpuError(PDU[21]);
                        }
                        else
                            _LastError = S7Consts.errIsoInvalidPDU;
                    }
                }
                Offset += DataSize;
                TotElements -= NumElements;
                Start += NumElements * WordSize;
            }

            if (_LastError == 0)
            {
                BytesWritten = Offset;
                Time_ms = Environment.TickCount - Elapsed;
            }
            else
                BytesWritten = 0;

            return _LastError;
        }

        public int ReadMultiVars(S7DataItem[] Items, int ItemsCount)
        {
            int Offset;
            int Length;
            int ItemSize;
            byte[] S7Item = new byte[12];
            byte[] S7ItemRead = new byte[1024]; 
            
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;

            // Checks items
            if (ItemsCount > MaxVars)
                return S7Consts.errCliTooManyItems;
            
            // Fills Header
            Array.Copy(S7_MRD_HEADER, 0, PDU, 0, S7_MRD_HEADER.Length);
            S7.SetWordAt(PDU, 13, (ushort)(ItemsCount * S7Item.Length + 2));
            PDU[18] = (byte)ItemsCount;
            // Fills the Items
            Offset = 19;
            for (int c = 0; c < ItemsCount; c++)
            {
                Array.Copy(S7_MRD_ITEM, S7Item, S7Item.Length);
                S7Item[3] = (byte)Items[c].WordLen;
                S7.SetWordAt(S7Item, 4, (ushort)Items[c].Amount);
                if (Items[c].Area == S7Consts.S7AreaDB)
                    S7.SetWordAt(S7Item, 6, (ushort)Items[c].DBNumber);
                S7Item[8] = (byte)Items[c].Area;
                
                // Address into the PLC
                int Address = Items[c].Start;
                S7Item[11] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                S7Item[10] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                S7Item[09] = (byte)(Address & 0x0FF);

                Array.Copy(S7Item, 0, PDU, Offset, S7Item.Length);
                Offset += S7Item.Length;
            }

            if (Offset > _PDULength)
                return S7Consts.errCliSizeOverPDU;

            S7.SetWordAt(PDU, 2, (ushort)Offset); // Whole size
            SendPacket(PDU, Offset);
            
            if (_LastError != 0)
                return _LastError;
            // Get Answer
            Length = RecvIsoPacket();
            if (_LastError != 0)
                return _LastError;
            // Check ISO Length
            if (Length < 22)
            {
                _LastError = S7Consts.errIsoInvalidPDU; // PDU too Small
                return _LastError;
            }
            // Check Global Operation Result
            _LastError = CpuError(S7.GetWordAt(PDU, 17));
            if (_LastError != 0)
                return _LastError;
            // Get true ItemsCount
            int ItemsRead = S7.GetByteAt(PDU, 20);
            if ((ItemsRead != ItemsCount) || (ItemsRead>MaxVars))
            {
                _LastError = S7Consts.errCliInvalidPlcAnswer;
                return _LastError;
            }
            // Get Data
            Offset = 21;           
            for (int c = 0; c < ItemsCount; c++)
            {
                // Get the Item
                Array.Copy(PDU, Offset, S7ItemRead, 0, Length-Offset);
                if (S7ItemRead[0] == 0xff)
                {
                    ItemSize = (int)S7.GetWordAt(S7ItemRead, 2);
                    if ((S7ItemRead[1] != TS_ResOctet) && (S7ItemRead[1] != TS_ResReal) && (S7ItemRead[1] != TS_ResBit))
                        ItemSize = ItemSize >> 3;
                    Marshal.Copy(S7ItemRead, 4, Items[c].pData, ItemSize);
                    Items[c].Result = 0;
                    if (ItemSize % 2 != 0)
                        ItemSize++; // Odd size are rounded
                    Offset = Offset + 4 + ItemSize;
                }
                else
                {
                    Items[c].Result = CpuError(S7ItemRead[0]);
                    Offset += 4; // Skip the Item header                           
                }
            }         
            Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int WriteMultiVars(S7DataItem[] Items, int ItemsCount)
        {
            int Offset;
            int ParLength;
            int DataLength;
            int ItemDataSize;
            byte[] S7ParItem = new byte[S7_MWR_PARAM.Length];
            byte[] S7DataItem = new byte[1024];

            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;

            // Checks items
            if (ItemsCount > MaxVars)
                return S7Consts.errCliTooManyItems;
            // Fills Header
            Array.Copy(S7_MWR_HEADER, 0, PDU, 0, S7_MWR_HEADER.Length);
            ParLength = ItemsCount * S7_MWR_PARAM.Length + 2;
            S7.SetWordAt(PDU, 13, (ushort)ParLength);
            PDU[18] = (byte)ItemsCount;
            // Fills Params
            Offset = S7_MWR_HEADER.Length;
            for (int c=0; c<ItemsCount; c++)
            {
                Array.Copy(S7_MWR_PARAM, 0, S7ParItem, 0, S7_MWR_PARAM.Length);
                S7ParItem[3] = (byte)Items[c].WordLen;
                S7ParItem[8] = (byte)Items[c].Area;
                S7.SetWordAt(S7ParItem, 4, (ushort)Items[c].Amount);
                S7.SetWordAt(S7ParItem, 6, (ushort)Items[c].DBNumber);
                // Address into the PLC
                int Address = Items[c].Start;
                S7ParItem[11] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                S7ParItem[10] = (byte)(Address & 0x0FF);
                Address = Address >> 8;
                S7ParItem[09] = (byte)(Address & 0x0FF);
                Array.Copy(S7ParItem, 0, PDU, Offset, S7ParItem.Length);
                Offset += S7_MWR_PARAM.Length;
            }
            // Fills Data
            DataLength = 0;
            for (int c = 0; c < ItemsCount; c++)
            {
                S7DataItem[0] = 0x00;
                switch (Items[c].WordLen)
                {
                    case S7Consts.S7WLBit:
                        S7DataItem[1] = TS_ResBit;
                        break;
                    case S7Consts.S7WLCounter:
                    case S7Consts.S7WLTimer:
                        S7DataItem[1] = TS_ResOctet;
                        break;
                    default:
                        S7DataItem[1] = TS_ResByte; // byte/word/dword etc.
                        break;
                };
                if ((Items[c].WordLen==S7Consts.S7WLTimer) || (Items[c].WordLen == S7Consts.S7WLCounter))
                    ItemDataSize = Items[c].Amount * 2;
                else
                    ItemDataSize = Items[c].Amount;

                if ((S7DataItem[1] != TS_ResOctet) && (S7DataItem[1] != TS_ResBit))
                    S7.SetWordAt(S7DataItem, 2, (ushort)(ItemDataSize*8));
                else
                    S7.SetWordAt(S7DataItem, 2, (ushort)ItemDataSize);

                Marshal.Copy(Items[c].pData, S7DataItem, 4, ItemDataSize);
                if (ItemDataSize % 2 != 0)
                {
                    S7DataItem[ItemDataSize+4] = 0x00;
                    ItemDataSize++;
                }
                Array.Copy(S7DataItem, 0, PDU, Offset, ItemDataSize+4);
                Offset = Offset + ItemDataSize + 4;
                DataLength = DataLength + ItemDataSize + 4;
            }

            // Checks the size
            if (Offset > _PDULength)
                return S7Consts.errCliSizeOverPDU;

            S7.SetWordAt(PDU, 2, (ushort)Offset); // Whole size
            S7.SetWordAt(PDU, 15, (ushort)DataLength); // Whole size
            SendPacket(PDU, Offset);

            RecvIsoPacket();
            if (_LastError==0)
            {
                // Check Global Operation Result
                _LastError = CpuError(S7.GetWordAt(PDU, 17));
                if (_LastError != 0)
                    return _LastError;
                // Get true ItemsCount
                int ItemsWritten = S7.GetByteAt(PDU, 20);
                if ((ItemsWritten != ItemsCount) || (ItemsWritten > MaxVars))
                {
                    _LastError = S7Consts.errCliInvalidPlcAnswer;
                    return _LastError;
                }

                for (int c=0; c<ItemsCount; c++)
                {
                    if (PDU[c + 21] == 0xFF)
                        Items[c].Result = 0;
                    else
                        Items[c].Result = CpuError((ushort)PDU[c + 21]);
                }
                Time_ms = Environment.TickCount - Elapsed;
            }
            return _LastError;
        }

        #endregion

        #region [Data I/O lean functions]

        public int DBRead(int DBNumber, int Start, int Size, byte[] Buffer)
        {
            return ReadArea(S7Consts.S7AreaDB, DBNumber, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int DBWrite(int DBNumber, int Start, int Size, byte[] Buffer)
        {
            return WriteArea(S7Consts.S7AreaDB, DBNumber, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int MBRead(int Start, int Size, byte[] Buffer)
        {
            return ReadArea(S7Consts.S7AreaMK, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int MBWrite(int Start, int Size, byte[] Buffer)
        {
            return WriteArea(S7Consts.S7AreaMK, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int EBRead(int Start, int Size, byte[] Buffer)
        {
            return ReadArea(S7Consts.S7AreaPE, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int EBWrite(int Start, int Size, byte[] Buffer)
        {
            return WriteArea(S7Consts.S7AreaPE, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int ABRead(int Start, int Size, byte[] Buffer)
        {
            return ReadArea(S7Consts.S7AreaPA, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int ABWrite(int Start, int Size, byte[] Buffer)
        {
            return WriteArea(S7Consts.S7AreaPA, 0, Start, Size, S7Consts.S7WLByte, Buffer);
        }

        public int TMRead(int Start, int Amount, ushort[] Buffer)
        {
            byte[] sBuffer = new byte[Amount * 2];
            int Result = ReadArea(S7Consts.S7AreaTM, 0, Start, Amount, S7Consts.S7WLTimer, sBuffer);
            if (Result == 0)
            {
                for (int c = 0; c < Amount; c++)
                {
                    Buffer[c] = (ushort)((sBuffer[c * 2 + 1] << 8) + (sBuffer[c * 2]));
                }
            }
            return Result;
        }

        public int TMWrite(int Start, int Amount, ushort[] Buffer)
        {
            byte[] sBuffer = new byte[Amount * 2];
            for (int c = 0; c < Amount; c++)
            {
                sBuffer[c * 2 + 1] = (byte)((Buffer[c] & 0xFF00) >> 8);
                sBuffer[c * 2] = (byte)(Buffer[c] & 0x00FF);
            }
            return WriteArea(S7Consts.S7AreaTM, 0, Start, Amount, S7Consts.S7WLTimer, sBuffer);
        }

        public int CTRead(int Start, int Amount, ushort[] Buffer)
        {
            byte[] sBuffer = new byte[Amount * 2];
            int Result = ReadArea(S7Consts.S7AreaCT, 0, Start, Amount, S7Consts.S7WLCounter, sBuffer);
            if (Result==0)
            {
                for (int c=0; c<Amount; c++)
                {
                    Buffer[c] = (ushort)((sBuffer[c * 2 + 1] << 8) + (sBuffer[c * 2]));
                }
            }
            return Result;
        }

        public int CTWrite(int Start, int Amount, ushort[] Buffer)
        {
            byte[] sBuffer = new byte[Amount * 2];
            for (int c = 0; c < Amount; c++)
            {
                sBuffer[c * 2 + 1] = (byte)((Buffer[c] & 0xFF00)>>8);
                sBuffer[c * 2]= (byte)(Buffer[c] & 0x00FF);
            }
            return WriteArea(S7Consts.S7AreaCT, 0, Start, Amount, S7Consts.S7WLCounter, sBuffer);
        }

        #endregion

        #region [Directory functions]

        public int ListBlocks(ref S7BlocksList List)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        private string SiemensTimestamp(long EncodedDate)
        {
            DateTime DT = new DateTime(1984, 1, 1).AddSeconds(EncodedDate*86400);
#if WINDOWS_UWP || NETFX_CORE
            return DT.ToString(System.Globalization.DateTimeFormatInfo.CurrentInfo.ShortDatePattern);
#else
            return DT.ToShortDateString();
#endif
        }

        public int GetAgBlockInfo(int BlockType, int BlockNum, ref S7BlockInfo Info)
        {
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;

            S7_BI[30] = (byte)BlockType;
            // Block Number
            S7_BI[31] = (byte)((BlockNum / 10000) + 0x30);
            BlockNum = BlockNum % 10000;
            S7_BI[32] = (byte)((BlockNum / 1000) + 0x30);
            BlockNum = BlockNum % 1000;
            S7_BI[33] = (byte)((BlockNum / 100) + 0x30);
            BlockNum = BlockNum % 100;
            S7_BI[34] = (byte)((BlockNum / 10) + 0x30);
            BlockNum = BlockNum % 10;
            S7_BI[35] = (byte)((BlockNum / 1) + 0x30);

            SendPacket(S7_BI);

            if (_LastError == 0)
            {
                int Length = RecvIsoPacket();
                if (Length > 32) // the minimum expected
                {
                    ushort Result = S7.GetWordAt(PDU, 27);
                    if (Result == 0)
                    {
                        Info.BlkFlags= PDU[42];
                        Info.BlkLang = PDU[43];
                        Info.BlkType = PDU[44];
                        Info.BlkNumber = S7.GetWordAt(PDU, 45);
                        Info.LoadSize = S7.GetDIntAt(PDU, 47);
                        Info.CodeDate = SiemensTimestamp(S7.GetWordAt(PDU, 59));
                        Info.IntfDate = SiemensTimestamp(S7.GetWordAt(PDU, 65)); 
                        Info.SBBLength = S7.GetWordAt(PDU, 67);
                        Info.LocalData = S7.GetWordAt(PDU, 71);
                        Info.MC7Size = S7.GetWordAt(PDU, 73);
                        Info.Author = S7.GetCharsAt(PDU, 75, 8).Trim(new char[]{(char)0});
                        Info.Family = S7.GetCharsAt(PDU, 83, 8).Trim(new char[]{(char)0});
                        Info.Header = S7.GetCharsAt(PDU, 91, 8).Trim(new char[]{(char)0}); 
                        Info.Version = PDU[99];
                        Info.CheckSum = S7.GetWordAt(PDU, 101);
                    }
                    else
                        _LastError = CpuError(Result);
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;

            return _LastError;
            
        }

        public int GetPgBlockInfo(ref S7BlockInfo Info, byte[] Buffer, int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int ListBlocksOfType(int BlockType, ushort[] List, ref int ItemsCount)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        #endregion

        #region [Blocks functions]

        public int Upload(int BlockType, int BlockNum, byte[] UsrData, ref int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int FullUpload(int BlockType, int BlockNum, byte[] UsrData, ref int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int Download(int BlockNum, byte[] UsrData, int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int Delete(int BlockType, int BlockNum)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int DBGet(int DBNumber, byte[] UsrData, ref int Size)
        {
            S7BlockInfo BI = new S7BlockInfo();
            int Elapsed = Environment.TickCount;
            Time_ms = 0;

            _LastError = GetAgBlockInfo(Block_DB, DBNumber, ref BI);

            if (_LastError==0)
            {
                int DBSize = BI.MC7Size;
                if (DBSize <= UsrData.Length)
                {
                    Size = DBSize;
                    _LastError = DBRead(DBNumber, 0, DBSize, UsrData);
                    if (_LastError == 0)
                        Size = DBSize;
                }
                else
                    _LastError = S7Consts.errCliBufferTooSmall;                  
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int DBFill(int DBNumber, int FillChar)
        {
            S7BlockInfo BI = new S7BlockInfo();
            int Elapsed = Environment.TickCount;
            Time_ms = 0;
            
            _LastError = GetAgBlockInfo(Block_DB, DBNumber, ref BI);
            
            if (_LastError == 0)
            {
                byte[] Buffer = new byte[BI.MC7Size];
                for (int c = 0; c < BI.MC7Size; c++)
                    Buffer[c] = (byte)FillChar;
                _LastError = DBWrite(DBNumber, 0, BI.MC7Size, Buffer);
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        #endregion

        #region [Date/Time functions]

        public int GetPlcDateTime(ref DateTime DT)
        {
            int Length;
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;

            SendPacket(S7_GET_DT);
            if (_LastError == 0)
            {
                Length = RecvIsoPacket();
                if (Length > 30) // the minimum expected
                {
                    if ((S7.GetWordAt(PDU, 27) == 0) && (PDU[29] == 0xFF))
                    {
                        DT = S7.GetDateTimeAt(PDU, 35);
                    }
                    else
                        _LastError = S7Consts.errCliInvalidPlcAnswer;
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }

            if(_LastError==0)
                Time_ms = Environment.TickCount - Elapsed;

            return _LastError;
        }

        public int SetPlcDateTime(DateTime DT)
        {
            int Length;
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;

            S7.SetDateTimeAt(S7_SET_DT, 31, DT);
            SendPacket(S7_SET_DT);
            if (_LastError == 0)
            {
                Length = RecvIsoPacket();
                if (Length > 30) // the minimum expected
                {
                    if (S7.GetWordAt(PDU, 27) != 0)
                        _LastError = S7Consts.errCliInvalidPlcAnswer;
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;

            return _LastError;
        }

        public int SetPlcSystemDateTime()
        {
            return SetPlcDateTime(DateTime.Now);
        }

        #endregion

        #region [System Info functions]

        public int GetOrderCode(ref S7OrderCode Info)
        {
            S7SZL SZL = new S7SZL();
            int Size = 1024;
            SZL.Data = new byte[Size];
            int Elapsed = Environment.TickCount;
            _LastError = ReadSZL(0x0011, 0x000, ref SZL, ref Size);
            if (_LastError == 0)
            {
                Info.Code = S7.GetCharsAt(SZL.Data, 2, 20);
                Info.V1 = SZL.Data[Size - 3];
                Info.V2 = SZL.Data[Size - 2];
                Info.V3 = SZL.Data[Size - 1];
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int GetCpuInfo(ref S7CpuInfo Info)
        {
            S7SZL SZL = new S7SZL();
            int Size = 1024;
            SZL.Data = new byte[Size];
            int Elapsed = Environment.TickCount;
            _LastError = ReadSZL(0x001C, 0x000, ref SZL, ref Size);
            if (_LastError == 0)
            {
                Info.ModuleTypeName = S7.GetCharsAt(SZL.Data, 172, 32);
                Info.SerialNumber = S7.GetCharsAt(SZL.Data, 138, 24);
                Info.ASName = S7.GetCharsAt(SZL.Data, 2, 24);
                Info.Copyright = S7.GetCharsAt(SZL.Data, 104, 26);
                Info.ModuleName = S7.GetCharsAt(SZL.Data, 36, 24);
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int GetCpInfo(ref S7CpInfo Info)
        {
            S7SZL SZL = new S7SZL();
            int Size = 1024;
            SZL.Data = new byte[Size];
            int Elapsed = Environment.TickCount;
            _LastError = ReadSZL(0x0131, 0x001, ref SZL, ref Size);
            if (_LastError == 0)
            {
                Info.MaxPduLength = S7.GetIntAt(PDU, 2);
                Info.MaxConnections = S7.GetIntAt(PDU, 4);
                Info.MaxMpiRate = S7.GetDIntAt(PDU, 6);
                Info.MaxBusRate = S7.GetDIntAt(PDU, 10);
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int ReadSZL(int ID, int Index, ref S7SZL SZL, ref int Size)
        {
            int Length;
            int DataSZL;
            int Offset = 0;
            bool Done = false;
            bool First = true;
            byte Seq_in = 0x00;
            ushort Seq_out = 0x0000;

            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;
            SZL.Header.LENTHDR = 0;

            do
            {
                if (First)
                {
                    S7.SetWordAt(S7_SZL_FIRST, 11, ++Seq_out);
                    S7.SetWordAt(S7_SZL_FIRST, 29, (ushort)ID);
                    S7.SetWordAt(S7_SZL_FIRST, 31, (ushort)Index);
                    SendPacket(S7_SZL_FIRST);
                }
                else
                {
                    S7.SetWordAt(S7_SZL_NEXT, 11, ++Seq_out);
                    PDU[24] = (byte)Seq_in;
                    SendPacket(S7_SZL_NEXT);
                }
                if (_LastError != 0)
                    return _LastError;

                Length = RecvIsoPacket();
                if (_LastError == 0)
                {
                    if (First)
                    {
                        if (Length > 32) // the minimum expected
                        {
                            if ((S7.GetWordAt(PDU, 27) == 0) && (PDU[29] == (byte)0xFF))
                            {
                                // Gets Amount of this slice
                                DataSZL = S7.GetWordAt(PDU, 31) - 8; // Skips extra params (ID, Index ...)
                                Done = PDU[26] == 0x00;
                                Seq_in = (byte)PDU[24]; // Slice sequence
                                SZL.Header.LENTHDR = S7.GetWordAt(PDU, 37);
                                SZL.Header.N_DR = S7.GetWordAt(PDU, 39);
                                Array.Copy(PDU, 41, SZL.Data, Offset, DataSZL);
                                //                                SZL.Copy(PDU, 41, Offset, DataSZL);
                                Offset += DataSZL;
                                SZL.Header.LENTHDR += SZL.Header.LENTHDR;
                            }
                            else
                                _LastError = S7Consts.errCliInvalidPlcAnswer;
                        }
                        else
                            _LastError = S7Consts.errIsoInvalidPDU;
                    }
                    else
                    {
                        if (Length > 32) // the minimum expected
                        {
                            if ((S7.GetWordAt(PDU, 27) == 0) && (PDU[29] == (byte)0xFF))
                            {
                                // Gets Amount of this slice
                                DataSZL = S7.GetWordAt(PDU, 31);
                                Done = PDU[26] == 0x00;
                                Seq_in = (byte)PDU[24]; // Slice sequence
                                Array.Copy(PDU, 37, SZL.Data, Offset, DataSZL);
                                Offset += DataSZL;
                                SZL.Header.LENTHDR += SZL.Header.LENTHDR;
                            }
                            else
                                _LastError = S7Consts.errCliInvalidPlcAnswer;
                        }
                        else
                            _LastError = S7Consts.errIsoInvalidPDU;
                    }
                }
                First = false;
            }
            while (!Done && (_LastError == 0));
            if (_LastError==0)
            {
                Size = SZL.Header.LENTHDR;
                Time_ms = Environment.TickCount - Elapsed;
            }
            return _LastError;
        }

        public int ReadSZLList(ref S7SZLList List, ref Int32 ItemsCount)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        #endregion

        #region [Control functions]

        public int PlcHotStart()
        {
            _LastError = 0;
            int Elapsed = Environment.TickCount;

            SendPacket(S7_HOT_START);
            if (_LastError == 0)
            {
                int Length = RecvIsoPacket();
                if (Length > 18) // 18 is the minimum expected
                {
                    if (PDU[19] != pduStart)
                        _LastError = S7Consts.errCliCannotStartPLC;
                    else
                    {
                        if (PDU[20] == pduAlreadyStarted)
                            _LastError = S7Consts.errCliAlreadyRun;
                        else
                            _LastError = S7Consts.errCliCannotStartPLC;
                    }
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int PlcColdStart()
        {
            _LastError = 0;
            int Elapsed = Environment.TickCount;

            SendPacket(S7_COLD_START);
            if (_LastError == 0)
            {
                int Length = RecvIsoPacket();
                if (Length > 18) // 18 is the minimum expected
                {
                    if (PDU[19] != pduStart)
                        _LastError = S7Consts.errCliCannotStartPLC;
                    else
                    {
                        if (PDU[20] == pduAlreadyStarted)
                            _LastError = S7Consts.errCliAlreadyRun;
                        else
                            _LastError = S7Consts.errCliCannotStartPLC;
                    }
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int PlcStop()
        {
            _LastError = 0;
            int Elapsed = Environment.TickCount;

            SendPacket(S7_STOP);
            if (_LastError == 0)
            {
                int Length = RecvIsoPacket();
                if (Length > 18) // 18 is the minimum expected
                {
                    if (PDU[19]!=pduStop)
                        _LastError = S7Consts.errCliCannotStopPLC;
                    else
                    { 
                        if (PDU[20]== pduAlreadyStopped)
                            _LastError = S7Consts.errCliAlreadyStop;
                        else
                            _LastError = S7Consts.errCliCannotStopPLC;
                    }
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int PlcCopyRamToRom(UInt32 Timeout)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int PlcCompress(UInt32 Timeout)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int PlcGetStatus(ref Int32 Status)
        {
            _LastError = 0;
            int Elapsed = Environment.TickCount;

            SendPacket(S7_GET_STAT);
            if (_LastError == 0)
            {
                int Length = RecvIsoPacket();
                if (Length > 30) // the minimum expected
                {
                    ushort Result = S7.GetWordAt(PDU, 27);
                    if (Result == 0)
                    {
                        switch (PDU[44])
                        {
                            case S7Consts.S7CpuStatusUnknown:
                            case S7Consts.S7CpuStatusRun:
                            case S7Consts.S7CpuStatusStop:
                                {
                                    Status = PDU[44];
                                    break;
                                }
                            default:
                                {
                                    // Since RUN status is always 0x08 for all CPUs and CPs, STOP status
                                    // sometime can be coded as 0x03 (especially for old cpu...)
                                    Status = S7Consts.S7CpuStatusStop;
                                    break;
                                }
                        }
                    }
                    else
                        _LastError = CpuError(Result);
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        #endregion

        #region [Security functions]
        public int SetSessionPassword(string Password)
        {
            byte[] pwd = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
            int Length;
            _LastError = 0;
            int Elapsed = Environment.TickCount;
            // Encodes the Password
            S7.SetCharsAt(pwd, 0, Password);
            pwd[0] = (byte)(pwd[0] ^ 0x55);
            pwd[1] = (byte)(pwd[1] ^ 0x55);
            for (int c = 2; c < 8; c++)
            {
                pwd[c] = (byte)(pwd[c] ^ 0x55 ^ pwd[c - 2]);
            }
            Array.Copy(pwd, 0, S7_SET_PWD, 29, 8);
            // Sends the telegrem
            SendPacket(S7_SET_PWD);
            if (_LastError == 0)
            {
                Length = RecvIsoPacket();
                if (Length > 32) // the minimum expected
                {
                    ushort Result = S7.GetWordAt(PDU, 27);
                    if (Result != 0)
                        _LastError = CpuError(Result);
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            return _LastError;
        }

        public int ClearSessionPassword()
        {
            int Length;
            _LastError = 0;
            int Elapsed = Environment.TickCount;
            SendPacket(S7_CLR_PWD);
            if (_LastError == 0)
            {
                Length = RecvIsoPacket();
                if (Length > 30) // the minimum expected
                {
                    ushort Result = S7.GetWordAt(PDU, 27);
                    if (Result != 0)
                        _LastError = CpuError(Result);
                }
                else
                    _LastError = S7Consts.errIsoInvalidPDU;
            }
            return _LastError;
        }

        public int GetProtection(ref S7Protection Protection)
        {
            S7Client.S7SZL SZL = new S7Client.S7SZL();
            int Size = 256;
            SZL.Data = new byte[Size];
            _LastError = ReadSZL(0x0232, 0x0004, ref SZL, ref Size);
            if (_LastError == 0)
            {
                Protection.sch_schal = S7.GetWordAt(SZL.Data, 2);
                Protection.sch_par = S7.GetWordAt(SZL.Data, 4);
                Protection.sch_rel = S7.GetWordAt(SZL.Data, 6);
                Protection.bart_sch = S7.GetWordAt(SZL.Data, 8);
                Protection.anl_sch = S7.GetWordAt(SZL.Data, 10);
            }
            return _LastError;
        }
        #endregion

        #region [Low Level]

        public int IsoExchangeBuffer(byte[] Buffer, ref Int32 Size)
        {
            _LastError = 0;
            Time_ms = 0;
            int Elapsed = Environment.TickCount;
            Array.Copy(TPKT_ISO, 0, PDU, 0, TPKT_ISO.Length);
            S7.SetWordAt(PDU, 2, (ushort)(Size + TPKT_ISO.Length));
            try
            {
                Array.Copy(Buffer, 0, PDU, TPKT_ISO.Length, Size);
            }
            catch
            {
                return S7Consts.errIsoInvalidPDU;
            }
            SendPacket(PDU, TPKT_ISO.Length + Size);
            if (_LastError==0)
            {
                int Length=RecvIsoPacket();
                if (_LastError==0)
                {
                    Array.Copy(PDU, TPKT_ISO.Length, Buffer, 0, Length - TPKT_ISO.Length);
                    Size = Length - TPKT_ISO.Length;
                }
            }
            if (_LastError == 0)
                Time_ms = Environment.TickCount - Elapsed;
            else
                Size = 0;
            return _LastError;
        }

        #endregion

        #region [Async functions (not implemented)]

        public int AsReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsWriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsDBRead(int DBNumber, int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsDBWrite(int DBNumber, int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsMBRead(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsMBWrite(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsEBRead(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsEBWrite(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsABRead(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsABWrite(int Start, int Size, byte[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsTMRead(int Start, int Amount, ushort[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsTMWrite(int Start, int Amount, ushort[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsCTRead(int Start, int Amount, ushort[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsCTWrite(int Start, int Amount, ushort[] Buffer)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsListBlocksOfType(int BlockType, ushort[] List)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsReadSZL(int ID, int Index, ref S7SZL Data, ref Int32 Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsReadSZLList(ref S7SZLList List, ref Int32 ItemsCount)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsUpload(int BlockType, int BlockNum, byte[] UsrData, ref int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsFullUpload(int BlockType, int BlockNum, byte[] UsrData, ref int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int ASDownload(int BlockNum, byte[] UsrData, int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsPlcCopyRamToRom(UInt32 Timeout)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsPlcCompress(UInt32 Timeout)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsDBGet(int DBNumber, byte[] UsrData, ref int Size)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public int AsDBFill(int DBNumber, int FillChar)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        public bool CheckAsCompletion(ref int opResult)
        {
            opResult = 0;
            return false;
        }

        public int WaitAsCompletion(int Timeout)
        {
            return S7Consts.errCliFunctionNotImplemented;
        }

        #endregion

        #region [Info Functions / Properties]

        public string ErrorText(int Error)
        {
            switch (Error)
            {
                case 0: return "OK";
                case S7Consts.errTCPSocketCreation: return "SYS : Error creating the Socket";
                case S7Consts.errTCPConnectionTimeout: return "TCP : Connection Timeout";
                case S7Consts.errTCPConnectionFailed: return "TCP : Connection Error";
                case S7Consts.errTCPReceiveTimeout: return "TCP : Data receive Timeout";
                case S7Consts.errTCPDataReceive: return "TCP : Error receiving Data";
                case S7Consts.errTCPSendTimeout: return "TCP : Data send Timeout";
                case S7Consts.errTCPDataSend: return "TCP : Error sending Data";
                case S7Consts.errTCPConnectionReset: return "TCP : Connection reset by the Peer";
                case S7Consts.errTCPNotConnected: return "CLI : Client not connected";
                case S7Consts.errTCPUnreachableHost: return "TCP : Unreachable host";
                case S7Consts.errIsoConnect: return "ISO : Connection Error";
                case S7Consts.errIsoInvalidPDU: return "ISO : Invalid PDU received";
                case S7Consts.errIsoInvalidDataSize: return "ISO : Invalid Buffer passed to Send/Receive";
                case S7Consts.errCliNegotiatingPDU: return "CLI : Error in PDU negotiation";
                case S7Consts.errCliInvalidParams: return "CLI : invalid param(s) supplied";
                case S7Consts.errCliJobPending: return "CLI : Job pending";
                case S7Consts.errCliTooManyItems: return "CLI : too may items (>20) in multi read/write";
                case S7Consts.errCliInvalidWordLen: return "CLI : invalid WordLength";
                case S7Consts.errCliPartialDataWritten: return "CLI : Partial data written";
                case S7Consts.errCliSizeOverPDU: return "CPU : total data exceeds the PDU size";
                case S7Consts.errCliInvalidPlcAnswer: return "CLI : invalid CPU answer";
                case S7Consts.errCliAddressOutOfRange: return "CPU : Address out of range";
                case S7Consts.errCliInvalidTransportSize: return "CPU : Invalid Transport size";
                case S7Consts.errCliWriteDataSizeMismatch: return "CPU : Data size mismatch";
                case S7Consts.errCliItemNotAvailable: return "CPU : Item not available";
                case S7Consts.errCliInvalidValue: return "CPU : Invalid value supplied";
                case S7Consts.errCliCannotStartPLC: return "CPU : Cannot start PLC";
                case S7Consts.errCliAlreadyRun: return "CPU : PLC already RUN";
                case S7Consts.errCliCannotStopPLC: return "CPU : Cannot stop PLC";
                case S7Consts.errCliCannotCopyRamToRom: return "CPU : Cannot copy RAM to ROM";
                case S7Consts.errCliCannotCompress: return "CPU : Cannot compress";
                case S7Consts.errCliAlreadyStop: return "CPU : PLC already STOP";
                case S7Consts.errCliFunNotAvailable: return "CPU : Function not available";
                case S7Consts.errCliUploadSequenceFailed: return "CPU : Upload sequence failed";
                case S7Consts.errCliInvalidDataSizeRecvd: return "CLI : Invalid data size received";
                case S7Consts.errCliInvalidBlockType: return "CLI : Invalid block type";
                case S7Consts.errCliInvalidBlockNumber: return "CLI : Invalid block number";
                case S7Consts.errCliInvalidBlockSize: return "CLI : Invalid block size";
                case S7Consts.errCliNeedPassword: return "CPU : Function not authorized for current protection level";
                case S7Consts.errCliInvalidPassword: return "CPU : Invalid password";
                case S7Consts.errCliNoPasswordToSetOrClear: return "CPU : No password to set or clear";
                case S7Consts.errCliJobTimeout: return "CLI : Job Timeout";
                case S7Consts.errCliFunctionRefused: return "CLI : function refused by CPU (Unknown error)";
                case S7Consts.errCliPartialDataRead: return "CLI : Partial data read";
                case S7Consts.errCliBufferTooSmall: return "CLI : The buffer supplied is too small to accomplish the operation";
                case S7Consts.errCliDestroying: return "CLI : Cannot perform (destroying)";
                case S7Consts.errCliInvalidParamNumber: return "CLI : Invalid Param Number";
                case S7Consts.errCliCannotChangeParam: return "CLI : Cannot change this param now";
                case S7Consts.errCliFunctionNotImplemented: return "CLI : Function not implemented";
                default: return "CLI : Unknown error (0x" + Convert.ToString(Error, 16) + ")";
            };
        }

        public int LastError()
        {
            return _LastError;
        }

        public int RequestedPduLength()
        {
            return _PduSizeRequested;
        }

        public int NegotiatedPduLength()
        {
            return _PDULength;
        }

        public int ExecTime()
        {
            return Time_ms;
        }

        public int ExecutionTime
        {
            get
            {
                return Time_ms;
            }
        }

        public int PduSizeNegotiated
        {
            get
            {
                return _PDULength;
            }
        }

        public int PduSizeRequested
        {
            get
            {
                return _PduSizeRequested;
            }
            set
            {
                if (value < MinPduSizeToRequest)
                    value = MinPduSizeToRequest;
                if (value > MaxPduSizeToRequest)
                    value = MaxPduSizeToRequest;
                _PduSizeRequested = value;       
            }
        }

        public int PLCPort
        {
            get
            {
                return _PLCPort;
            }
            set
            {
                _PLCPort = value;
            }
        }

        public int ConnTimeout
        {
            get
            {
                return _ConnTimeout;
            }
            set
            {
                _ConnTimeout = value;
            }
        }

        public int RecvTimeout
        {
            get
            {
                return _RecvTimeout;
            }
            set
            {
                _RecvTimeout = value;
            }
        }

        public int SendTimeout
        {
            get
            {
                return _SendTimeout;
            }
            set
            {
                _SendTimeout = value;
            }
        }

        public bool Connected
        {
            get
            {
                return (Socket != null) && (Socket.Connected);
            }
        }


        #endregion
    }
}
