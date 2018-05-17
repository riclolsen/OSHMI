// screen_list.js
// Warning:
// There can't be any spaces after the "\" character in the lines below

optionhtml = "\
<option id='SELTELA_OPC1' selected disabled='disabled'>Escolha uma tela</option>\
<optgroup label='Centro de Telecomando'>\
  <option value='../svg/tela_tronco_coi-new.svg'      >COI..........[1]</option>\
  <option value='../svg/tela_tronco_coi_kv-new.svg'   >COI KV.......[2]</option>\
  <option value='../svg/tela_z_coi_kv.svg'     >COI KV Nova..[3]</option>\
  <option value='../svg/tela_tronco_coi_barra-new.svg'>COI Barras...[4]</option>\
</optgroup>\
<optgroup label='Console 1'>\
  <option value='../svg/tela_cbo-new.svg'             >CBO..........{Q}</option>\
  <option value='../svg/tela_cna1-new.svg'            >CNA1.........{W}</option>\
  <option value='../svg/tela_laj2-new.svg'            >LAJ2.........{E}</option>\
  <option value='../svg/tela_laj2_sa-new.svg'         >LAJ2 SA.........</option>\
  <option value='../svg/tela_laj2-prep.svg'           >LAJ2 PREP.RECOMP</option>\
  <option value='../svg/tela_sch-new.svg'             >SCH..........{R}</option>\
  <option value='../svg/tela_sch_sa-new.svg'          >SCH SA..........</option>\
  <option value='../svg/tela_sch2-new.svg'            >SCH2.........{T}</option>\
  <option value='../svg/tela_scr1-new.svg'            >SCR1.........{Y}</option>\
</optgroup>\
<optgroup label='Console 2'>\
  <option value='../svg/tela_cna2-new.svg'            >CNA2.........{A}</option>\
  <option value='../svg/tela_cna3-new.svg'            >CNA3.........{S}</option>\
  <option value='../svg/tela_cna3_sa-new.svg'         >CNA3 SA.........</option>\
  <option value='../svg/tela_gra3-new.svg'            >GRA3.........{D}</option>\
  <option value='../svg/tela_gua2-new.svg'            >GUA2.........{F}</option>\
  <option value='../svg/tela_lve2-new.svg'            >LVE2.........{G}</option>\
  <option value='../svg/tela_mis-new.svg'             >MIS..........{H}</option>\
  <option value='../svg/tela_nsr-new.svg'             >NSR..........{I}</option>\
  <option value='../svg/tela_oso2-new.svg'            >OSO2.........{J}</option>\
  <option value='../svg/tela_pal8-new.svg'            >PAL8.........{K}</option>\
  <option value='../svg/tela_via3-new.svg'            >VIA3.........{L}</option>\
  <option value='../svg/tela_via3_sa-new.svg'         >VIA3 SA.........</option>\
</optgroup>\
<optgroup label='Console 3'>\
  <option value='../svg/tela_ciag-new.svg'            >CIAG.........{Z}</option>\
  <option value='../svg/tela_eld-new.svg'             >ELD..........{X}</option>\
  <option value='../svg/tela_pa10-new.svg'            >PAL10........{C}</option>\
  <option value='../svg/tela_pa13-new.svg'            >PAL13........{V}</option>\
  <option value='../svg/tela_vip-new.svg'             >VIP..........{B}</option>\
  <option value='../svg/tela_gar1-new.svg'            >GAR1............</option>\
</optgroup>\
  <optgroup label='Outras SEs'>\
  <option value='../svg/tela_cal1-new.svg'            >CAL1............</option>\
  <option value='../svg/tela_cam-new.svg'             >CAM.............</option>\
  <option value='../svg/tela_cam3-new.svg'            >CAM3............</option>\
  <option value='../svg/tela_cax-new.svg'             >CAX.............</option>\
  <option value='../svg/tela_cax5-new.svg'            >CAX5............</option>\
  <option value='../svg/tela_cax6-new.svg'            >CAX6............</option>\
  <option value='../svg/tela_cin-new.svg'             >CIN.............</option>\
  <option value='../svg/tela_ctt-new.svg'             >CTT.............</option>\
  <option value='../svg/tela_ere1-new.svg'            >ERE1............</option>\
  <option value='../svg/tela_gra2-new.svg'            >GRA2............</option>\
  <option value='../svg/tela_iju1-new.svg'            >IJU1............</option>\
  <option value='../svg/tela_iju1_sa-new.svg'         >IJU1 SA.........</option>\
  <option value='../svg/tela_iju2-new.svg'            >IJU2............</option>\
  <option value='../svg/tela_grt-new.svg'             >GRT.............</option>\
  <option value='../svg/tela_mbr-new.svg'             >MBR.............</option>\
  <option value='../svg/tela_npe2-new.svg'            >NPE2............</option>\
  <option value='../svg/tela_pan-new.svg'             >PAN.............</option>\
  <option value='../svg/tela_pel3-new.svg'            >PEL3............</option>\
  <option value='../svg/tela_res-new.svg'             >RES.............</option>\
  <option value='../svg/tela_res_sa-new.svg'          >RES SA..........</option>\
  <option value='../svg/tela_sbo2-new.svg'            >SBO2............</option>\
  <option value='../svg/tela_sma1-new.svg'            >SMA1............</option>\
  <option value='../svg/tela_svi-new.svg'             >SVI.............</option>\
  <option value='../svg/tela_tpr2-new.svg'            >TPR2............</option>\
</optgroup>\
";