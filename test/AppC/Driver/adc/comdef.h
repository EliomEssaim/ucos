/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

comdef.h

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/**===========================================================================

 Revisions of comdef.h
 Version  		Name       	Date		Description
 1.0  		Eric	  	04/22/2004  	Initial Version  

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef COMDEF_H 
#define COMDEF_H

#define inpb(port)       (*((volatile unsigned char *) (port)))
#define inpw(port)       (*((volatile unsigned int *) (port)))
#define inph(port)       (*((volatile unsigned short *) (port)))

#define outpb(port, val)  (*((volatile unsigned char *) (port)) = ((unsigned char) (val)))
#define outpw(port, val) (*((volatile unsigned int *) (port)) = ((unsigned int) (val)))
#define outph(port, val) (*((volatile unsigned short *) (port)) = ((unsigned short) (val)))

#define MA_INB( io )  (unsigned char) inpb( io )
#define MA_INW( io )  (unsigned int) inpw( io )
#define MA_INH( io )  (unsigned short) inph( io )

#define MA_INBM( io, mask ) ( inpb( io ) & (mask) )
#define MA_INWM( io, mask ) ( inpw( io ) & (mask) )
#define MA_INHM( io, mask ) ( inph( io ) & (mask) )

#define MA_OUTB( io, val )  (void) outpb( io, (int) val)
#define MA_OUTW( io, val )  (void) outpw( io, (int) val)
#define MA_OUTH( io, val )  (void) outph( io, (int) val)

//======================================================================
//���ܣ�	
//����дʱ�䣺2006��2��20��
//���룺        io, mask, val
//�����        ȡval��mask��0λ���Լ�io��maskֵΪ0λ������õ�
//              �����io����Ӧ��
//======================================================================
#define MA_OUTBM( io, mask, val ) \
{\
  unsigned char temp;\
  (temp) =(((MA_INW(io) & (unsigned char)(~(mask))) | ((unsigned char)((val) & (mask)))));\
  ((void) outpb( io, (unsigned char)(temp)));\
}\
  
#define MA_OUTWM( io, mask, val) \
  {\
  unsigned int temp;\
  (temp) =(((MA_INW(io) & (unsigned int)(~(mask))) |((unsigned int)((val) & (mask)))));\
  (void) outpw( io, (unsigned int)(temp));\
  }\
  
#define MA_OUTHM( io, mask, val) \
  {\
  unsigned short temp;\
  (temp) =(((MA_INH(io) & (unsigned short)(~(mask))) |((unsigned short)((val) & (mask)))));\
  (void) outph( io, (unsigned short)(temp));\
  }\

#endif  /*COMDEF_H*/

