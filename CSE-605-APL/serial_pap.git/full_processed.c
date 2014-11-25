 -----------------------------------------------------------------------------------------
 | |STX|MSG_LEN|AC_ID|MSG_ID|-----Actual Msg----|ck_a|ck_b|                                |
 | total length= len(stx)+len(MSG_LEN)+len(AC_ID)+len(MSG_ID)+len(MSG)+len(ck_a)+len(ck_b) |
 ----------------------------------------------------------------------------------------
int main(){
  static uint8_t i; 
  int16_t climb = -gps.ned_vel.z; 
  int16_t course = (DegOfRad(gps.course)/((int32_t)1e6)); 
  if (UART1CheckFreeSpace((uint8_t)(((0+1+4+4+2+4+2+2+2+4+1+1 +2)+4)))) { 
	    downlink_nb_bytes += ((0+1+4+4+2+4+2+2+2+4+1+1 +2)+4);; 
	    downlink_nb_msgs++; 
	    UART1Transmit(0x99); 
	     uint8_t msg_len = ((0+1+4+4+2+4+2+2+2+4+1+1 +2)+4); 
	     UART1Transmit(msg_len);  
	     ck_a = msg_len;  
	    ck_b = msg_len;
	    ck_a += AC_ID; ck_b += ck_a; UART1Transmit(AC_ID); 
	    ck_a += 8; ck_b += ck_a; 
	    UART1Transmit(8);  

	    uint8_t _x = *((const uint8_t*)(&gps.fix)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    
	    uint8_t _x = *((const uint8_t*)(&gps.utm_pos.east)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.utm_pos.east)+1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.utm_pos.east)+2); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);  
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(const uint8_t*)(&gps.utm_pos.east)+2 +1);
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);


	    uint8_t _x = *((const uint8_t*)(&gps.utm_pos.north)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.utm_pos.north)+1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.utm_pos.north)+2); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(const uint8_t*)(&gps.utm_pos.north)+2 +1);
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
		  

	    uint8_t _x = *((const uint8_t*)(&course));
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);   
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&course)+1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);


	    uint8_t _x = *((const uint8_t*)(&gps.hmsl)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.hmsl)+1);
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.hmsl)+2); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(const uint8_t*)(&gps.hmsl)+2 +1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);

	    uint8_t _x = *((const uint8_t*)(&gps.gspeed)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.gspeed)+1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);;  


	    uint8_t _x = *((const uint8_t*)(&climb)); 
	    ck_a += _x; ck_b += ck_a;
	    UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&climb)+1);
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);

	    uint8_t _x = *((const uint8_t*)(&gps.week));  ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.week)+1);
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	      
	      
	    uint8_t _x = *((const uint8_t*)(&gps.tow)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.tow)+1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(&gps.tow)+2); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);
	    uint8_t _x = *((const uint8_t*)(const uint8_t*)(const uint8_t*)(&gps.tow)+2 +1); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x); 

	    uint8_t _x = *((const uint8_t*)(&gps.utm_pos.zone)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);

	    uint8_t _x = *((const uint8_t*)(&i)); 
	    ck_a += _x; ck_b += ck_a; UART1Transmit(_x);

	    UART1Transmit(ck_a); UART1Transmit(ck_b); UART1SendMessage(); 
	    else 
	      downlink_nb_ovrn++; i++; 
	      
  }
  

