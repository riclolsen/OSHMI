/*
    OSHMI - Open Substation HMI
    S7 Protocol Driver
	Copyright 2019 - Ricardo L. Olsen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

    THIS SOFTWARE USES PROJECT Sharp7 By Davide Nardella 

 */

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using Sharp7;
using Newtonsoft.Json;

class OSHMI_S7Client
{
    static public string Version = "OSHMI S7 Protocol Driver Version 0.1 ( Uses Project Sharp7 By Davide Nardella ).";
    static public string ConfigFile = "c:\\oshmi\\conf\\s7client.ini";
    static public int UDPPortSend = 9100;
    static public int UDPPortListen = 9101;
    static public int NumberOfServers = 0;
    static public bool logread = true;
    static public bool logcommand = true;
    static public ConcurrentQueue<OSHMI_control> ControlQueue = new ConcurrentQueue<OSHMI_control>();
    public struct OSHMI_control
    {
        public string oshmi_cmd_tag;
        public uint oshmi_point_key;
        public double value;
        public long timestamp;
        public int countdown;
        public List<string> servers_checked;
    }
    public class OSHMI_control_msg
    {
        public string tag;
        public uint point_key;
        public double value;
    }

    public struct S7RW
    {
        public string name;
        public string data_type;
        public uint data_area;
        public uint db;
        public uint offset;
        public uint amount;
        public uint point_number;
        public uint delay_ms;
        public uint bit;
    }

    public struct S7PLC
    {
        public string ip;
        public string server_name;
        public uint connection_type;
        public uint rack;
        public uint slot;
        public uint local_tsap;
        public uint remote_tsap;
        public string password;
        public List<S7RW> reads;
        public List<S7RW> writes;
    }

    static void SendUdp(byte[] data)
    {
        using (UdpClient c = new UdpClient())
            c.Send(data, data.Length, "127.0.0.1", UDPPortSend);
    }

    // Convert S7 data type name to Word Lenght Code
    static uint StringToWordLengthCode( string wordlengthstr )
    {
        wordlengthstr = wordlengthstr.Trim().ToUpper();
        switch (wordlengthstr)
        {
            case "BIT":
                return S7Consts.S7WLBit;
            case "BYTE":
                return S7Consts.S7WLByte;
            case "CHAR":
                return S7Consts.S7WLChar;
            case "WORD":
                return S7Consts.S7WLWord;
            case "DWORD":
                return S7Consts.S7WLDWord;
            case "INT":
                return S7Consts.S7WLInt;
            case "DINT":
                return S7Consts.S7WLDInt;
            case "REAL":
                return S7Consts.S7WLReal;
            case "COUNTER":
                return S7Consts.S7WLCounter;
            case "TIMER":
                return S7Consts.S7WLTimer;
        }

        return S7Consts.S7WLBit;
    }

    // Converts a numeric or hex (like "0x01") string to uint
    static uint StringDHToUint( string snum )
    {
        if (snum == null || snum == "")
            return 0;

        uint res = 0;
        snum = snum.Trim().ToLower();
        if (snum.Contains("0x"))
            res = Convert.ToUInt32(snum.Substring(2), 16);
        else
            res = Convert.ToUInt32(snum);
        return res;     
    }

    //------------------------------------------------------------------------------
    // HexDump, a very nice function, it's not mine.
    // I found it on the net somewhere some time ago... thanks to the author ;-)
    //------------------------------------------------------------------------------
    static void HexDump(byte[] bytes, int Size)
    {
        if (bytes == null)
            return;
        int bytesLength = Size;
        int bytesPerLine = 16;

        char[] HexChars = "0123456789ABCDEF".ToCharArray();

        int firstHexColumn =
              8                   // 8 characters for the address
            + 3;                  // 3 spaces

        int firstCharColumn = firstHexColumn
            + bytesPerLine * 3       // - 2 digit for the hexadecimal value and 1 space
            + (bytesPerLine - 1) / 8 // - 1 extra space every 8 characters from the 9th
            + 2;                  // 2 spaces 

        int lineLength = firstCharColumn
            + bytesPerLine           // - characters to show the ascii value
            + Environment.NewLine.Length; // Carriage return and line feed (should normally be 2)

        char[] line = (new String(' ', lineLength - 2) + Environment.NewLine).ToCharArray();
        int expectedLines = (bytesLength + bytesPerLine - 1) / bytesPerLine;
        StringBuilder result = new StringBuilder(expectedLines * lineLength);

        for (int i = 0; i < bytesLength; i += bytesPerLine)
        {
            line[0] = HexChars[(i >> 28) & 0xF];
            line[1] = HexChars[(i >> 24) & 0xF];
            line[2] = HexChars[(i >> 20) & 0xF];
            line[3] = HexChars[(i >> 16) & 0xF];
            line[4] = HexChars[(i >> 12) & 0xF];
            line[5] = HexChars[(i >> 8) & 0xF];
            line[6] = HexChars[(i >> 4) & 0xF];
            line[7] = HexChars[(i >> 0) & 0xF];

            int hexColumn = firstHexColumn;
            int charColumn = firstCharColumn;

            for (int j = 0; j < bytesPerLine; j++)
            {
                if (j > 0 && (j & 7) == 0) hexColumn++;
                if (i + j >= bytesLength)
                {
                    line[hexColumn] = ' ';
                    line[hexColumn + 1] = ' ';
                    line[charColumn] = ' ';
                }
                else
                {
                    byte b = bytes[i + j];
                    line[hexColumn] = HexChars[(b >> 4) & 0xF];
                    line[hexColumn + 1] = HexChars[b & 0xF];
                    line[charColumn] = (b < 32 ? '·' : (char)b);
                }
                hexColumn += 3;
                charColumn++;
            }
            result.Append(line);
#if __MonoCS__
            result.Append('\n');
#endif
        }
        Console.WriteLine(result.ToString());
    }

    static void CheckCommand(S7Client client, S7PLC plc)
    {
        // check for controls
        OSHMI_control oc;
        bool found = false;
        if (ControlQueue.TryDequeue(out oc))
        {
            DateTime dt = DateTime.Now;

            if (dt.Ticks / 10000000 < oc.timestamp / 10000000 + 10) // past ten seconds expires the command (discards it)
            {
                if (!oc.servers_checked.Contains(plc.server_name))
                {
                    foreach (S7RW write in plc.writes)
                    {
                        if (write.point_number == oc.oshmi_point_key)
                        {
                            found = true;
                            byte[] buffer = { 0, 0, 0, 0 };
                            ushort[] usbuffer = { 0 };

                            switch (write.data_type)
                            {
                                case "BIT":
                                    S7.SetBitAt(ref buffer, 0, System.Convert.ToInt32(write.bit), System.Convert.ToBoolean(oc.value));
                                    break;
                                case "BYTE":
                                    S7.SetByteAt(buffer, 0, System.Convert.ToByte(oc.value));
                                    break;
                                case "CHAR":
                                    S7.SetByteAt(buffer, 0, System.Convert.ToByte(oc.value));
                                    break;
                                case "WORD":
                                    S7.SetWordAt(buffer, 0, System.Convert.ToUInt16(oc.value));
                                    break;
                                case "DWORD":
                                    S7.SetDWordAt(buffer, 0, System.Convert.ToUInt32(oc.value));
                                    break;
                                case "INT":
                                    S7.SetIntAt(buffer, 0, System.Convert.ToInt16(oc.value));
                                    break;
                                case "DINT":
                                    S7.SetDIntAt(buffer, 0, System.Convert.ToInt32(oc.value));
                                    break;
                                case "REAL":
                                    S7.SetRealAt(buffer, 0, System.Convert.ToSingle(oc.value));
                                    break;
                                case "COUNTER":
                                    S7.SetCounterAt(usbuffer, 0, System.Convert.ToUInt16(oc.value));
                                    buffer[1] = (byte)((usbuffer[0] & 0xFF00) >> 8);
                                    buffer[0] = (byte)(usbuffer[0] & 0x00FF);
                                    break;
                                case "TIMER":
                                    S7.SetWordAt(buffer, 0, System.Convert.ToUInt16(oc.value));
                                    break;
                                default:
                                    Console.WriteLine(plc.server_name + " Write unsupported data type: " + write.data_type);
                                    return;
                            }

                            client.WriteArea( System.Convert.ToInt32(write.data_area),
                                              System.Convert.ToInt32(write.db),
                                              System.Convert.ToInt32(write.offset), 
                                              1,
                                              System.Convert.ToInt32(StringToWordLengthCode(write.data_type)),
                                              buffer);
                            
                            Console.Write( plc.server_name + " WRITEN COMMAND TAG " + oc.oshmi_cmd_tag + " KEY " + oc.oshmi_point_key + 
                                           " VAL " + oc.value + " " + write.data_type );
                            break;
                        }
                    }
                    if (!found)
                    {
                        oc.countdown--;
                        if (oc.countdown > 0)
                        {
                            oc.servers_checked.Add(plc.server_name); // mark as checked
                            ControlQueue.Enqueue(oc); // return command object to the queue if not the last to check
                        }
                        else
                        if (logcommand) 
                            Console.Write(plc.server_name + " DISCARDED COMMAND (NOT FOUND) ON TAG " + oc.oshmi_cmd_tag + " KEY " + oc.oshmi_point_key + " VAL " + oc.value);
                    }
                }
                else
                {
                    ControlQueue.Enqueue(oc); // return command object to the queue if not the last to check
                }
            }
            else
            if (logcommand) 
                Console.Write(plc.server_name + " DISCARDED COMMAND (TIMEOUT) ON TAG " + oc.oshmi_cmd_tag + " KEY " + oc.oshmi_point_key + " VAL " + oc.value);
        }
    }

    // Thread to Process PLC Read Queue
    static void ProcessPLC(S7PLC plc)
    {
        do
        {
            try
            {
                System.Threading.Thread.Sleep(5000);
                S7Client client = new S7Client();
                client.SetConnectionType(System.Convert.ToUInt16(plc.connection_type));
                client.SetConnectionParams(plc.ip, System.Convert.ToUInt16(plc.local_tsap), System.Convert.ToUInt16(plc.remote_tsap));
                Console.WriteLine("Try to connect " + plc.server_name);
                int res = client.Connect();
                if (res != 0 || client.Connected == false)
                    continue;

                Console.WriteLine("Connected " + plc.server_name + " IP:" + plc.ip);
                S7Client.S7CpuInfo Info = new S7Client.S7CpuInfo();
                res = client.GetCpuInfo(ref Info);
                if (res == 0)
                {
                    Console.WriteLine(plc.server_name + "  Module Type Name : " + Info.ModuleTypeName);
                    Console.WriteLine(plc.server_name + "  Serial Number    : " + Info.SerialNumber);
                    Console.WriteLine(plc.server_name + "  AS Name          : " + Info.ASName);
                    Console.WriteLine(plc.server_name + "  Module Name      : " + Info.ModuleName);
                };
                System.Threading.Thread.Sleep(1000);

                while (client.Connected)
                foreach (S7RW read in plc.reads)
                {
                    CheckCommand(client, plc);
                    byte[] Buffer = new byte[1024]; 
                    int BytesRead = 0;
                    res = client.ReadArea( 
                           System.Convert.ToInt32(read.data_area),
                           System.Convert.ToInt32(read.db),
                           System.Convert.ToInt32(read.offset),
                           System.Convert.ToInt32(read.amount),
                           System.Convert.ToInt32(StringToWordLengthCode(read.data_type)), 
                           Buffer,
                           ref BytesRead);
                        if (res == 0)
                        {
                            // HexDump(Buffer, BytesRead);
                            for (int i = 0; i < read.amount; i++)
                            {
                                int bytecnt;
                                double value = 0;
                                switch (read.data_type)
                                {
                                    case "BIT":
                                        bytecnt = i / 8;
                                        value = System.Convert.ToDouble(S7.GetBitAt(Buffer, bytecnt, i % 8));
                                        break;
                                    case "BYTE":
                                        bytecnt = i;
                                        value = S7.GetByteAt(Buffer, bytecnt);
                                        break;
                                    case "CHAR":
                                        bytecnt = i;
                                        value = S7.GetByteAt(Buffer, bytecnt);
                                        break;
                                    case "WORD":
                                        bytecnt = i * 2;
                                        value = S7.GetWordAt(Buffer, bytecnt);
                                        break;
                                    case "DWORD":
                                        bytecnt = i * 4;
                                        value = S7.GetDWordAt(Buffer, bytecnt);
                                        break;
                                    case "INT":
                                        bytecnt = i * 2;
                                        value = S7.GetIntAt(Buffer, bytecnt);
                                        break;
                                    case "DINT":
                                        bytecnt = i * 4;
                                        value = S7.GetDIntAt(Buffer, bytecnt);
                                        break;
                                    case "REAL":
                                        bytecnt = i * 4;
                                        value = S7.GetRealAt(Buffer, bytecnt);
                                        break;
                                    case "COUNTER":
                                        bytecnt = i * 2;
                                        value = S7.GetCounter((ushort)((Buffer[bytecnt + 1] << 8) + (Buffer[bytecnt])));
                                        break;
                                    case "TIMER":
                                        bytecnt = i * 2;
                                        value = (ushort)((Buffer[bytecnt + 1] << 8) + (Buffer[bytecnt]));
                                        break;
                                    default:
                                        Console.WriteLine("Unsupported data type: " + read.data_type);
                                        break;
                                }
                                SendUdp(Encoding.ASCII.GetBytes(
                                    "[{\"point_key\":" + (read.point_number + i) +
                                    ",\"value\":" + value +
                                    ",\"failed\":false" +
                                    "}]"));
                                if (logread)
                                    Console.WriteLine(plc.server_name + " " + read.name + " OSHMI_POINT_NUMBER=" + (read.point_number + i) + " VALUE=" + value + " " + read.data_type);
                            }
                        }
                        else
                        {
                            Console.WriteLine(plc.server_name + " Error " + read.name);
                        }
                        System.Threading.Thread.Sleep(System.Convert.ToInt32(read.delay_ms));
                    }                

                // client.Disconnect();
                Console.WriteLine("Disconnected " + plc.server_name);
            }
            catch (Exception e)
            {
                // EXCEPTION HANDLER
                Console.WriteLine("Exception " + plc.server_name);
                Console.WriteLine(e);
            }
            System.Threading.Thread.Sleep(10000);
        } while (true);
    }

    static void ListenUDP()
    {
        UdpClient listener = new UdpClient(UDPPortListen);
        IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, UDPPortListen);

        try
        {
            while (true)
            {
                // Waiting for messages
                byte[] bytes = listener.Receive(ref groupEP);

                Console.WriteLine($"Received message from {groupEP} : ");
                string s = Encoding.UTF8.GetString(bytes, 0, bytes.Length - 1).Trim(); // bytes.Length-1 is to remove the final \0
                Console.WriteLine(s);

                try
                {
                    string tag = "";
                    double dval = 0;
                    uint point_key = 0;
                    OSHMI_control_msg result = JsonConvert.DeserializeObject<OSHMI_control_msg>(s);
                    tag = result.tag;
                    point_key = result.point_key;
                    dval = result.value;

                    DateTime currentDate = DateTime.Now;
                    OSHMI_control oc = 
                        new OSHMI_control() 
                        { 
                            oshmi_cmd_tag = tag, 
                            oshmi_point_key = point_key, 
                            value = dval, 
                            timestamp = currentDate.Ticks, 
                            countdown = NumberOfServers, 
                            servers_checked = new List<string>() 
                        };
                    ControlQueue.Enqueue(oc);
                }
                catch (Exception e)
                {
                    Console.WriteLine("ERROR: Invalid JSON Message!");
                    Console.WriteLine(e.Message);
                }
            }
        }
        catch (SocketException e)
        {
            Console.WriteLine(e);
        }
        finally
        {
            listener.Close();
        }
    }

    //-------------------------------------------------------------------------  
    // Main                                  
    //-------------------------------------------------------------------------  
    public static void Main(string[] args)
    {
        List<S7PLC> servers = new List<S7PLC>();
        IniParser ini = new IniParser(ConfigFile);
        string test = ini.GetSetting("RTU_2", "IP", "");
        Console.WriteLine(test);

        for (int i=1; i<100; i++)
        {
            string rtu = "RTU_" + i.ToString();
            string ip = ini.GetSetting(rtu, "IP", "");
            if (ip == null || ip == "")
                break;
            NumberOfServers += 1;
            string server_name = rtu;
            uint connection_type = StringDHToUint(ini.GetSetting(rtu, "CONNECTION_TYPE", "2"));
            uint rack = StringDHToUint(ini.GetSetting(rtu, "RACK", "0"));
            uint slot = StringDHToUint(ini.GetSetting(rtu, "SLOT", "0"));
            uint local_tsap = StringDHToUint(ini.GetSetting(rtu, "LOCAL_TSAP", "0x200"));
            uint remote_tsap = StringDHToUint(ini.GetSetting(rtu, "REMOTE_TSAP", "0x200"));
            string password = ini.GetSetting(rtu, "PASSWORD", "");
            List<S7RW> reads = new List<S7RW>();
            List<S7RW> writes = new List<S7RW>();

            for (int j = 1; j < 10000; j++)
            {
                string line = ini.GetSetting(rtu, "READ_" + j.ToString(), "");
                if ( line == null || line == "")
                    break;
                string[] arrstr = Regex.Split(line, "\\s+");
                if (arrstr.Length != 7)
                    break;
                string name = "READ_" + j.ToString();
                string data_type = arrstr[0];
                uint data_area = StringDHToUint(arrstr[1]);
                uint db = StringDHToUint(arrstr[2]);
                uint offset = StringDHToUint(arrstr[3]);
                uint amount = StringDHToUint(arrstr[4]);
                uint point_number = StringDHToUint(arrstr[5]);
                uint delay_ms = StringDHToUint(arrstr[6]);
                S7RW s7r = new S7RW
                    {
                       name = name,
                       data_type = data_type,
                       data_area = data_area,
                       db = db,
                       offset = offset,
                       amount = amount,
                       point_number = point_number,
                       delay_ms = delay_ms
                };
                reads.Add(s7r);
            }

            for (int j = 1; j < 10000; j++)
            {
                string line = ini.GetSetting(rtu, "WRITE_" + j.ToString(), "");
                if (line == null || line == "")
                    break;
                string[] arrstr = Regex.Split(line, "\\s+");
                if (arrstr.Length != 6)
                    break;
                string name = "WRITE_" + j.ToString();
                string data_type = arrstr[0];
                uint data_area = StringDHToUint(arrstr[1]);
                uint db = StringDHToUint(arrstr[2]);
                uint offset = StringDHToUint(arrstr[3]);
                uint amount = 1;
                uint point_number = StringDHToUint(arrstr[4]);
                uint bit = StringDHToUint(arrstr[5]);
                S7RW s7w = new S7RW
                {
                    name = name,
                    data_type = data_type,
                    data_area = data_area,
                    db = db,
                    offset = offset,
                    amount = amount,
                    point_number = point_number,
                    bit = bit
                };
                writes.Add(s7w);
            }

            S7PLC s7plc = new S7PLC
            {
                ip = ip,
                server_name = server_name,
                connection_type = connection_type,
                rack = rack,
                slot = slot,
                local_tsap = local_tsap,
                remote_tsap = remote_tsap,
                password = password,
                reads = reads,
                writes = writes
            };
            servers.Add(s7plc);
        }

        foreach (S7PLC srv in servers)
        {
            Thread t = new Thread(() => ProcessPLC(srv));
            t.Start();
        }

        Thread u = new Thread(() => ListenUDP());
        u.Start();

        Console.WriteLine(Version);
        Console.WriteLine("Press any key to end.");
        Console.ReadKey();
        Environment.Exit(Environment.ExitCode);                                         
    }
}
