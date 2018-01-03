// MachiKania�p�R���|�W�b�g�J���[�M���o�̓v���O����
//�@PIC32MX170F256B�p�O���t�B�b�N���C�u�����@by K.Tanaka

#include "videoout.h"

void g_pset(int x,int y,unsigned int c)
// (x,y)�̈ʒu�ɃJ���[c�œ_��`��
{
	unsigned short *ad,d1;

	if((unsigned int)x>=G_X_RES) return;
	if((unsigned int)y>=G_Y_RES) return;
	c&=15;
	ad=gVRAM+y*G_H_WORD+x/4;
	d1=~(0xf000>>(x%4)*4);
	c<<=(3-(x%4))*4;
	*ad&=d1;
	*ad|=c;
}

// ��m*�cn�h�b�g�̃L�����N�^�[�����Wx,y�ɕ\��
// unsigned char bmp[m*n]�z��ɁA�P���ɃJ���[�ԍ�����ׂ�
// �J���[�ԍ���0�̕����͓����F�Ƃ��Ĉ���
void g_putbmpmn(int x,int y,char m,char n,const unsigned char bmp[])
{
	int i,j,k;
	unsigned short *vp;
	const unsigned char *p;
	if(x<=-m || x>=G_X_RES || y<=-n || y>=G_Y_RES) return; //��ʊO
	if(m<=0 || n<=0) return;
	if(y<0){ //��ʏ㕔�ɐ؂��ꍇ
		i=0;
		p=bmp-y*m;
	}
	else{
		i=y;
		p=bmp;
	}
	for(;i<y+n;i++){
		if(i>=G_Y_RES) return; //��ʉ����ɐ؂��ꍇ
		if(x<0){ //��ʍ��ɐ؂��ꍇ�͎c�镔���̂ݕ`��
			j=0;
			p+=-x;
			vp=gVRAM+i*G_H_WORD;
		}
		else{
			j=x;
			vp=gVRAM+i*G_H_WORD+x/4;
		}
		k=j%4;
		if(k!=0){ //16�r�b�g���E�̍����`��
			if(k==1){
				if(*p) *vp=(*vp&0xf0ff)|(*p<<8);
				p++;
				j++;
				if(j>=x+m) continue;
			}
			if(k<=2){
				if(*p) *vp=(*vp&0xff0f)|(*p<<4);
				p++;
				j++;
				if(j>=x+m) continue;
			}
			if(*p) *vp=(*vp&0xfff0)|*p;
			p++;
			j++;
			if(j>=x+m) continue;
			vp++;
		}
		//��������16�r�b�g���E�̓�������щE���`��
		while(1){
			if(j>=G_X_RES){ //��ʉE�ɐ؂��ꍇ
				p+=x+m-j;
				break;
			}
			if(*p) *vp=(*vp&0x0fff)|(*p<<12);
			p++;
			j++;
			if(j>=x+m) break;
			if(*p) *vp=(*vp&0xf0ff)|(*p<<8);
			p++;
			j++;
			if(j>=x+m) break;
			if(*p) *vp=(*vp&0xff0f)|(*p<<4);
			p++;
			j++;
			if(j>=x+m) break;
			if(*p) *vp=(*vp&0xfff0)|*p;
			p++;
			j++;
			if(j>=x+m) break;
			vp++;
		}
	}
}

// �cm*��n�h�b�g�̃L�����N�^�[����
// �J���[0�œh��Ԃ�
void g_clrbmpmn(int x,int y,char m,char n)
{
	int i,j,k;
	unsigned short mask,*vp;

	if(x<=-m || x>=G_X_RES || y<=-n || y>=G_Y_RES) return; //��ʊO
	if(m<=0 || n<=0) return;
	if(y<0){ //��ʏ㕔�ɐ؂��ꍇ
		i=0;
	}
	else{
		i=y;
	}
	for(;i<y+n;i++){
		if(i>=G_Y_RES) return; //��ʉ����ɐ؂��ꍇ
		if(x<0){ //��ʍ��ɐ؂��ꍇ�͎c�镔���̂ݏ���
			j=0;
			vp=gVRAM+i*G_H_WORD;
		}
		else{
			j=x;
			vp=gVRAM+i*G_H_WORD+x/4;
		}
		k=j%4;
		if(k!=0){ //16�r�b�g���E�̍�������
			if(k==1){
				*vp&=0xf0ff;
				j++;
				if(j>=x+m) continue;
			}
			if(k<=2){
				*vp&=0xff0f;
				j++;
				if(j>=x+m) continue;
			}
			*vp&=0xfff0;
			j++;
			if(j>=x+m) continue;
			vp++;
		}
		//��������16�r�b�g���E�̓�������
		for(;j<x+m-3;j+=4){
			if(j>=G_X_RES) break; //��ʉE�ɐ؂��ꍇ
			*vp++=0;
		}
		if(j<G_X_RES){
			k=x+m-j;
			if(k!=0){ //16�r�b�g���E�̉E������
				mask=0xffff>>(k*4);
				*vp&=mask;
			}
		}
	}
}

void g_gline(int x1,int y1,int x2,int y2,unsigned int c)
// (x1,y1)-(x2,y2)�ɃJ���[c�Ő�����`��
{
	int sx,sy,dx,dy,i;
	int e;

	if(x2>x1){
		dx=x2-x1;
		sx=1;
	}
	else{
		dx=x1-x2;
		sx=-1;
	}
	if(y2>y1){
		dy=y2-y1;
		sy=1;
	}
	else{
		dy=y1-y2;
		sy=-1;
	}
	if(dx>=dy){
		e=-dx;
		for(i=0;i<=dx;i++){
			g_pset(x1,y1,c);
			x1+=sx;
			e+=dy*2;
			if(e>=0){
				y1+=sy;
				e-=dx*2;
			}
		}
	}
	else{
		e=-dy;
		for(i=0;i<=dy;i++){
			g_pset(x1,y1,c);
			y1+=sy;
			e+=dx*2;
			if(e>=0){
				x1+=sx;
				e-=dy*2;
			}
		}
	}
}
void g_hline(int x1,int x2,int y,unsigned int c)
// (x1,y)-(x2,y)�ւ̐������C���������`��
{
	int temp;
	unsigned short d,*ad;

	if(y<0 || y>=G_Y_RES) return;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=G_X_RES) return;
	if(x1<0) x1=0;
	if(x2>=G_X_RES) x2=G_X_RES-1;
	while(x1&3){
		g_pset(x1++,y,c);
		if(x1>x2) return;
	}
	d=c|(c<<4)|(c<<8)|(c<<12);
	ad=gVRAM+y*G_H_WORD+x1/4;
	while(x1+3<=x2){
		*ad++=d;
		x1+=4;
	}
	while(x1<=x2) g_pset(x1++,y,c);
}

void g_circle(int x0,int y0,unsigned int r,unsigned int c)
// (x0,y0)�𒆐S�ɁA���ar�A�J���[c�̉~��`��
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		g_pset(x0-x,y0-y,c);
		g_pset(x0-x,y0+y,c);
		g_pset(x0+x,y0-y,c);
		g_pset(x0+x,y0+y,c);
		g_pset(x0-y,y0-x,c);
		g_pset(x0-y,y0+x,c);
		g_pset(x0+y,y0-x,c);
		g_pset(x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}
void g_boxfill(int x1,int y1,int x2,int y2,unsigned int c)
// (x1,y1),(x2,y2)��Ίp���Ƃ���J���[c�œh��ꂽ�����`��`��
{
	int temp;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=G_X_RES) return;
	if(y1>y2){
		temp=y1;
		y1=y2;
		y2=temp;
	}
	if(y2<0 || y1>=G_Y_RES) return;
	if(y1<0) y1=0;
	if(y2>=G_Y_RES) y2=G_Y_RES-1;
	while(y1<=y2){
		g_hline(x1,x2,y1++,c);
	}
}
void g_circlefill(int x0,int y0,unsigned int r,unsigned int c)
// (x0,y0)�𒆐S�ɁA���ar�A�J���[c�œh��ꂽ�~��`��
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		g_hline(x0-x,x0+x,y0-y,c);
		g_hline(x0-x,x0+x,y0+y,c);
		g_hline(x0-y,x0+y,y0-x,c);
		g_hline(x0-y,x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}

void g_putfont(int x,int y,unsigned int c,int bc,unsigned char n)
//8*8�h�b�g�̃A���t�@�x�b�g�t�H���g�\��
//���W�ix,y)�A�J���[�ԍ�c
//bc:�o�b�N�O�����h�J���[�A�����̏ꍇ����
//n:�����ԍ�
{
	int i,j,k;
	unsigned int d1,mask;
	unsigned char d;
	const unsigned char *p;
	unsigned short *ad;

//	p=FontData+n*8;
	p=fontp+n*8;
	c&=15;
	if(bc>=0) bc&=15;
	if(x<0 || x+7>=G_X_RES || y<0 || y+7>=G_Y_RES){
		for(i=0;i<8;i++){
			d=*p++;
			for(j=0;j<8;j++){
				if(d&0x80) g_pset(x+j,y+i,c);
				else if(bc>=0) g_pset(x+j,y+i,bc);
				d<<=1;
			}
		}
		return;
	}
	ad=gVRAM+y*G_H_WORD+x/4;
	k=x&3;
	for(i=0;i<8;i++){
		d=*p;
		d1=0;
		mask=0;
		for(j=0;j<8;j++){
			d1<<=4;
			mask<<=4;
			if(d&0x80) d1+=c;
			else if(bc>=0) d1+=bc;
			else mask+=15;
			d<<=1;
		}
		if(k==0){
			if(bc>=0){
				*ad=d1>>16;
				*(ad+1)=(unsigned short)d1;
			}
			else{
				*ad=(*ad & (mask>>16))|(d1>>16);
				*(ad+1)=(*(ad+1) & (unsigned short)mask)|(unsigned short)d1;
			}
		}
		else if(k==1){
			if(bc>=0){
				*ad=(*ad & 0xf000)|(d1>>20);
				*(ad+1)=(unsigned short)(d1>>4);
				*(ad+2)=(*(ad+2) & 0x0fff)|((unsigned short)d1<<12);
			}
			else{
				*ad=(*ad & ((mask>>20)|0xf000))|(d1>>20);
				*(ad+1)=(*(ad+1) & (unsigned short)(mask>>4))|(unsigned short)(d1>>4);
				*(ad+2)=(*(ad+2) & (((unsigned short)mask<<12)|0x0fff))|((unsigned short)d1<<12);
			}
		}
		else if(k==2){
			if(bc>=0){
				*ad=(*ad & 0xff00)|(d1>>24);
				*(ad+1)=(unsigned short)(d1>>8);
				*(ad+2)=(*(ad+2) & 0x00ff)|((unsigned short)d1<<8);
			}
			else{
				*ad=(*ad & ((mask>>24)|0xff00))|(d1>>24);
				*(ad+1)=(*(ad+1) & (unsigned short)(mask>>8))|(unsigned short)(d1>>8);
				*(ad+2)=(*(ad+2) & (((unsigned short)mask<<8)|0x00ff))|((unsigned short)d1<<8);
			}
		}
		else{
			if(bc>=0){
				*ad=(*ad & 0xfff0)|(d1>>28);
				*(ad+1)=(unsigned short)(d1>>12);
				*(ad+2)=(*(ad+2) & 0x000f)|((unsigned short)d1<<4);
			}
			else{
				*ad=(*ad & ((mask>>28)|0xfff0))|(d1>>28);
				*(ad+1)=(*(ad+1) & (unsigned short)(mask>>12))|(unsigned short)(d1>>12);
				*(ad+2)=(*(ad+2) & (((unsigned short)mask<<4)|0x000f))|((unsigned short)d1<<4);
			}
		}
		p++;
		ad+=G_H_WORD;
	}
}

void g_printstr(int x,int y,unsigned int c,int bc,unsigned char *s){
	//���W(x,y)����J���[�ԍ�c�ŕ�����s��\���Abc:�o�b�N�O�����h�J���[
	while(*s){
		g_putfont(x,y,c,bc,*s++);
		x+=8;
	}
}
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n){
	//���W(x,y)�ɃJ���[�ԍ�c�Ő��ln��\���Abc:�o�b�N�O�����h�J���[
	unsigned int d,e;
	d=10;
	e=0;
	while(n>=d){
		e++;
		if(e==9) break;
		d*=10;
	}	
	x+=e*8;
	do{
		g_putfont(x,y,c,bc,'0'+n%10);
		n/=10;
		x-=8;
	}while(n!=0);
}
void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e){
	//���W(x,y)�ɃJ���[�ԍ�c�Ő��ln��\���Abc:�o�b�N�O�����h�J���[�Ae���ŕ\��
	if(e==0) return;
	x+=(e-1)*8;
	do{
		g_putfont(x,y,c,bc,'0'+n%10);
		e--;
		n/=10;
		x-=8;
	}while(e!=0 && n!=0);
	while(e!=0){
		g_putfont(x,y,c,bc,' ');
		x-=8;
		e--;
	}
}
unsigned int g_color(int x,int y){
	//���W(x,y)��VRAM��̌��݂̃p���b�g�ԍ���Ԃ��A��ʊO��0��Ԃ�
	unsigned short *ad;

	if((unsigned int)x>=G_X_RES) return 0;
	if((unsigned int)y>=G_Y_RES) return 0;
	ad=gVRAM+y*G_H_WORD+x/4;
	return (*ad >>(3-(x&3))*4) & 0xf;
}