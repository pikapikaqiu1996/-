//define.h
unsigned char asctodtmf[]={0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0x0c,0,0,0,0,
						   0,0,0x0b,0,0,0,0,0,0x0a,0x01,
						   0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0,0,
						   0,0,0,0,0,0x0d,0x0e,0x0f,0x00};
unsigned char dtmftoasc[]={'D','1','2','3','4','5','6','7','8','9',
                           '0','*','#','A','B','C'};
CString  strAlarmType[]={"温度告警","湿度告警","红外告警","门禁告警","烟雾告警","浸水告警","毒气告警","电缆断线","交流断电","交流恢复","直流电源低"};
