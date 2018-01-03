/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka
   http://www.ze.em-net.ne.jp/~kenken/index.html
*/
// �L�[���́A�J�[�\���\���֘A�@�\ by K.Tanaka
// PS/2�L�[�{�[�h���̓V�X�e���A�J���[�e�L�X�g�o�̓V�X�e�����p

#include "videoout.h"
#include "ps2keyboard.h"
#include "keyinput.h"

unsigned char blinkchar,blinkcolor,insertmode,blinktimer;

void getcursorchar(){
// �J�[�\���_�ŗp�Ɍ��̕����R�[�h��ޔ�
	blinkchar=*cursor;
	if(twidth==WIDTH_X1) blinkcolor=*(cursor+ATTROFFSET1);
	else blinkcolor=*(cursor+ATTROFFSET2);
}
void resetcursorchar(){
// �J�[�\�������̕����ɖ߂�
	*cursor=blinkchar;
	if(twidth==WIDTH_X1) *(cursor+ATTROFFSET1)=blinkcolor;
	else *(cursor+ATTROFFSET2)=blinkcolor;
}
void blinkcursorchar(){
// ����I�ɌĂяo�����ƂŃJ�[�\����_�ŕ\��������
// BLINKTIME�œ_�ŊԊu��ݒ�
// ���O��getcursorchar()���Ăяo���Ă���
	blinktimer++;
	if(blinktimer>=BLINKTIME*2) blinktimer=0;
	if(blinktimer<BLINKTIME){
		if(insertmode) *cursor=CURSORCHAR;
		else *cursor=CURSORCHAR2;
		if(twidth==WIDTH_X1) *(cursor+ATTROFFSET1)=CURSORCOLOR;
		else *(cursor+ATTROFFSET2)=CURSORCOLOR;
	}
	else{
		*cursor=blinkchar;
		if(twidth==WIDTH_X1) *(cursor+ATTROFFSET1)=blinkcolor;
		else *(cursor+ATTROFFSET2)=blinkcolor;
	}
}

unsigned char _MIPS32 inputchar(void){
// �L�[�{�[�h����1�L�[���͑҂�
// �߂�l �ʏ핶���̏ꍇASCII�R�[�h�A���̑���0�A�O���[�o���ϐ�vkey�ɉ��z�L�[�R�[�h
	unsigned char k;
	unsigned short d;
	d=drawcount;
	while(1){
		while(d==drawcount) asm("wait"); //60����1�b�E�F�C�g
		d=drawcount;
		k=ps2readkey();  //�L�[�o�b�t�@����ǂݍ��݁Ak1:�ʏ핶�����͂̏ꍇASCII�R�[�h
		if(vkey) return k;
	}
}

unsigned char _MIPS32 cursorinputchar(void){
// �J�[�\���\�����Ȃ���L�[�{�[�h����1�L�[���͑҂�
// �߂�l �ʏ핶���̏ꍇASCII�R�[�h�A���̑���0�A�O���[�o���ϐ�vkey�ɉ��z�L�[�R�[�h
	unsigned char k;
	unsigned short d;
	getcursorchar(); //�J�[�\���ʒu�̕�����ޔ��i�J�[�\���_�ŗp�j
	d=drawcount;
	while(1){
		while(d==drawcount) asm("wait"); //60����1�b�E�F�C�g
		d=drawcount;
		blinkcursorchar(); //�J�[�\���_�ł�����
		k=ps2readkey();  //�L�[�o�b�t�@����ǂݍ��݁Ak1:�ʏ핶�����͂̏ꍇASCII�R�[�h
		if((vkey&0xFF) == VK_RETURN||
		   (vkey&0xFF) == VK_SEPARATOR){
		  k = '\n';
		}else if((vkey&0xFF) == VK_TAB){
		  k = '\t';
		}else if((vkey&0xFF) == VK_BACK){
		  k = '\b';
		}
		if(k) break;  //�L�[�������ꂽ�ꍇ���[�v���甲����
	}
	resetcursorchar(); //�J�[�\�������̕����\���ɖ߂�
	return k;
}

unsigned char printinputchar(void){
// �J�[�\���\�����Ȃ���L�[�{�[�h����ʏ핶���L�[���͑҂����A���͂��ꂽ������\��
// �߂�l ���͂��ꂽ������ASCII�R�[�h�A�O���[�o���ϐ�vkey�ɍŌ�ɉ����ꂽ�L�[�̉��z�L�[�R�[�h
	unsigned char k;
	while(1){
		k=cursorinputchar();
		if(k) break;
	}
	printchar(k);
	return k;
}






