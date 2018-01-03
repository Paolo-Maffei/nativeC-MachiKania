// �e�L�X�gVRAM�R���|�W�b�g�J���[�o�̓v���O���� PIC32MX170F256B�p�@by K.Tanaka Rev.6
// RAM�t�H���g�Ή��A�O���t�B�b�N�Ή��A6�h�b�g�t�H���g�Ή�
// �o�� PORTB
// ��30�~�c27�L�����N�^�[�A256�F�J���[�p���b�g�@����40�L�����N�^�[�Ή�
// �N���b�N���g��3.579545MHz�~16�{�@�i�O���t�B�b�N���́~15�{�j
//�@�J���[�M���̓J���[�T�u�L�����A��90�x���Ƃɏo�́A270�x��1�h�b�g�i4����3�����j

#include "videoout.h"
#include <plib.h>

// �e�L�X�g���[�h 5bit DA�M���萔�f�[�^
#define C_SYN	0
#define C_BLK	6
#define C_BST1	6
#define C_BST2	3
#define C_BST3	6
#define C_BST4	9

// �e�L�X�g���[�h �p���X���萔
#define V_NTSC		262					// 262�{/���
#define V_SYNC		10					// ���������{��
#define V_PREEQ		26					// �u�����L���O��ԏ㑤�iV_SYNC�{V_PREEQ�͋����Ƃ��邱�Ɓj
#define V_LINE		(WIDTH_Y*8)			// �摜�`����
#define H_NTSC		3632				// ��63.5��sec
#define H_SYNC		269					// �����������A��4.7��sec
#define H_CBST		304					// �J���[�o�[�X�g�J�n�ʒu�i�������������肩��j
#define H_BACK		339					// ���X�y�[�X�i�������������オ�肩��j


// �O���t�B�b�N���[�h 5bit DA�M���萔�f�[�^
//#define GC_SYN	0
//#define GC_BLK	6
#define GC_BST1	6
#define GC_BST2	3
#define GC_BST3	8

// �O���t�B�b�N���[�h �p���X���萔
//#define G_V_NTSC		262					// 262�{/���
//#define G_V_SYNC		10					// ���������{��
#define G_V_PREEQ		18					// �u�����L���O��ԏ㑤
#define G_V_LINE		G_Y_RES				// �摜�`����
#define G_H_NTSC		3405				// ��63.5��sec
#define G_H_SYNC		252					// �����������A��4.7��sec
#define G_H_CBST		285					// �J���[�o�[�X�g�J�n�ʒu�i�������������肩��j
#define G_H_BACK		483					// ���X�y�[�X�i�������������オ�肩��j


// �O���[�o���ϐ���`
// unsigned short GVRAM[G_H_WORD*G_Y_RES] __attribute__ ((aligned (4))); //�O���t�B�b�NVRAM�i�e�X�g�p�j

unsigned int ClTable[256];	//�J���[�p���b�g�M���e�[�u���A�e�F32bit�����ʂ���8bit�����ɏo��
//unsigned char TVRAM[WIDTH_X*WIDTH_Y*2+1] __attribute__ ((aligned (4)));
unsigned char TVRAM[WIDTH_X2*WIDTH_Y*2+1] __attribute__ ((aligned (4)));
unsigned short *gVRAM; //�O���t�B�b�NVRAM�J�n�ʒu�̃|�C���^
unsigned char *VRAMp; //VRAM�Ə�����VRAM�A�h���X�B�e�L�X�gVRAM��1�o�C�g�̃_�~�[�K�v
unsigned char *fontp; //�t�H���g�i�[�A�h���X�A����������FontData�ARAM�w�肷�邱�Ƃ�PCG������

unsigned int bgcolor;		// �o�b�N�O�����h�J���[�g�`�f�[�^�A32bit�����ʂ���8bit�����ɏo��
volatile unsigned short LineCount;	// �������̍s
volatile unsigned short drawcount;	//�@1��ʕ\���I�����Ƃ�1�����B�A�v������0�ɂ���B
					// �Œ�1��͉�ʕ\���������Ƃ̃`�F�b�N�ƁA�A�v���̏���������ʊ��ԕK�v���̊m�F�ɗ��p�B
volatile char drawing;		//�@�f����ԏ�������-1�A���̑���0

unsigned char twidth; //�e�L�X�g1�s�������i30 or 40�j
unsigned char graphmode; //�e�L�X�g���[�h�� 0�A�O���t�B�b�N���[�h�� 0�ȊO

//�O���t�B�b�N���[�h�ł̃J���[�M���e�[�u��
//16�F���̃J���[�p���b�g�e�[�u��
//5�h�b�g���̐M���f�[�^��ێ��B���ɏo�͂���
//�ʑ���15����0,4,9,13,3,7,12,1,6,10�̏�
unsigned char gClTable[16*16];

/**********************
*  Timer2 ���荞�ݏ����֐�
***********************/
void __ISR(8, ipl5) T2Handler(void)
{
	asm volatile("#":::"a0");
	asm volatile("#":::"v0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("addiu	$a0,$a0,-22");
	asm volatile("bltz	$a0,label1_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label1_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label1");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

	asm volatile("label1:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label1_2:");
	//LATB=C_SYN;
	asm volatile("addiu	$a0,$zero,%0"::"n"(C_SYN));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$a0,0($v0)");// �����M������������B��������ɑS�Ă̐M���o�͂̃^�C�~���O�𒲐�����

	if(!graphmode){
		if(LineCount<V_SYNC){
			// ������������
			OC3R = H_NTSC-H_SYNC-7;	// �؂荞�݃p���X���ݒ�
			OC3CON = 0x8001;
		}
		else{
			OC1R = H_SYNC-7;		// �����p���X��4.7usec
			OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
			if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
				// �摜�`����
				OC2R = H_SYNC+H_BACK-7-47;// �摜�M���J�n�̃^�C�~���O
				OC2CON = 0x8001;		// �^�C�}2�I�������V���b�g
				if(LineCount==V_SYNC+V_PREEQ){
					VRAMp=TVRAM;
					drawing=-1;
				}
				else{
					if((LineCount-(V_SYNC+V_PREEQ))%8==0){
						 //1�L�����N�^�c8�h�b�g
						VRAMp+=twidth;
					}
				}
			}
			else if(LineCount==V_SYNC+V_PREEQ+V_LINE){ // 1��ʍŌ�̕`��I��
				drawing=0;
				drawcount++;
			}
		}
	}
	else{
		if(LineCount<V_SYNC){
			// ������������
			OC3R = G_H_NTSC-G_H_SYNC-7;	// �؂荞�݃p���X���ݒ�
			OC3CON = 0x8001;
		}
		else{
			OC1R = G_H_SYNC-7;		// �����p���X��4.7usec
			OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
			if(LineCount>=V_SYNC+G_V_PREEQ && LineCount<V_SYNC+G_V_PREEQ+G_V_LINE){
				// �摜�`����
				OC2R = G_H_SYNC+G_H_BACK-7-4-20;// �摜�M���J�n�̃^�C�~���O
				OC2CON = 0x8001;		// �^�C�}2�I�������V���b�g
				if(LineCount==V_SYNC+G_V_PREEQ){
					VRAMp=(unsigned char *)gVRAM;
					drawing=-1; // �摜�`����
				}
				else{
					VRAMp+=G_H_WORD*2;// ���̍s��
				}
			}
			else if(LineCount==V_SYNC+G_V_PREEQ+G_V_LINE){
				drawing=0;
				drawcount++;
			}
		}
	}
	LineCount++;
	if(LineCount>=V_NTSC) LineCount=0;
	IFS0bits.T2IF = 0;			// T2���荞�݃t���O�N���A
}

/*********************
*  OC3���荞�ݏ����֐� ���������؂荞�݃p���X
*********************/
void __ISR(14, ipl5) OC3Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("la	$v0,%0"::"i"(&graphmode));
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("nop");
	asm volatile("bnez	$v1,label4_3");
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(H_NTSC-H_SYNC+18)));
	asm volatile("bltz	$a0,label4_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label4_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label4");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label4_3:");
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(G_H_NTSC-G_H_SYNC+18)));
	asm volatile("bltz	$a0,label4_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label4_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label4");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label4:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label4_2:");
	// �����M���̃��Z�b�g
	//	LATB=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));

	//�����M�����Z�b�g�B�e�L�X�g���[�h�ł͓����M�����������肩��3363�T�C�N��
	//�O���t�B�b�N���[�h�ł͐M�����������肩��3153�T�C�N��
	asm volatile("sb	$v1,0($v0)");

	IFS0bits.OC3IF = 0;			// OC3���荞�݃t���O�N���A
}

/*********************
*  OC1���荞�ݏ����֐� �������������オ��`�J���[�o�[�X�g
*********************/
void __ISR(6, ipl5) OC1Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("la	$v0,%0"::"i"(&graphmode));
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("nop");
	asm volatile("bnez	$v1,label2_3"); //�O���t�B�b�N���[�h
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(H_SYNC+18)));
	asm volatile("bltz	$a0,label2_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label2_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label2");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label2:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label2_2:");
	// �����M���̃��Z�b�g
	//	LATB=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����������������肩��269�T�C�N��

	// 28�N���b�N�E�F�C�g
	asm volatile("addiu	$a0,$zero,9");
asm volatile("loop2:");
	asm volatile("addiu	$a0,$a0,-1");
	asm volatile("nop");
	asm volatile("bnez	$a0,loop2");

	// �J���[�o�[�X�g�M�� 9�����o��
	asm volatile("addiu	$a0,$zero,4*9");
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("lui	$v1,%0"::"n"(C_BST3+(C_BST4<<8)));
	asm volatile("ori	$v1,$v1,%0"::"n"(C_BST1+(C_BST2<<8)));
asm volatile("loop3:");
	asm volatile("addiu	$a0,$a0,-1");	//���[�v�J�E���^
	asm volatile("sb	$v1,0($v0)");	// �J���[�o�[�X�g�J�n�B�����������������肩��304�T�C�N��
	asm volatile("rotr	$v1,$v1,8");
	asm volatile("bnez	$a0,loop3");
	asm volatile("nop");
	asm volatile("b		oc1end");

// �O���t�B�b�N���[�h
asm volatile("label2_3:");
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(G_H_SYNC+18)));
	asm volatile("bltz	$a0,label2_5");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label2_5");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label2_4");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label2_4:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label2_5:");
	// �����M���̃��Z�b�g
	//	LATB=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATB));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����������������肩��252�T�C�N��

	// 28�N���b�N�E�F�C�g
	asm volatile("addiu	$a0,$zero,9");
asm volatile("loop2_1:");
	asm volatile("addiu	$a0,$a0,-1");
	asm volatile("nop");
	asm volatile("bnez	$a0,loop2_1");

	// �J���[�o�[�X�g�M�� 9�����o��
	asm volatile("addiu	$a0,$zero,9");
	asm volatile("la	$v0,%0"::"i"(&LATB));
asm volatile("loop3_1:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(GC_BST1));
	asm volatile("sb	$v1,0($v0)");	// �J���[�o�[�X�g�J�n�B�����������������肩��285�T�C�N��
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(GC_BST2));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("addiu	$v1,$zero,%0"::"n"(GC_BST3));
	asm volatile("sb	$v1,0($v0)");
	asm volatile("addiu	$a0,$a0,-1");//���[�v�J�E���^
	asm volatile("nop");
	asm volatile("bnez	$a0,loop3_1");

asm volatile("oc1end:");
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$v1,0($v0)");

	IFS0bits.OC1IF = 0;			// OC1���荞�݃t���O�N���A

}

/***********************
*  OC2���荞�ݏ����֐��@�f���M���o��
***********************/
void __ISR(10, ipl5) OC2Handler(void)
{
// �f���M���o��
	//�C�����C���A�Z���u���ł̔j�󃌃W�X�^��錾�i�X�^�b�N�ޔ������邽�߁j
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");
	asm volatile("#":::"a1");
	asm volatile("#":::"a2");
	asm volatile("#":::"t0");
	asm volatile("#":::"t1");
	asm volatile("#":::"t2");
	asm volatile("#":::"t3");
	asm volatile("#":::"t4");
	asm volatile("#":::"t5");
	asm volatile("#":::"t6");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("la	$v0,%0"::"i"(&graphmode));
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("nop");
	asm volatile("bnez	$v1,jump3");//�O���t�B�b�N���[�h�̏ꍇjump3��
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(H_SYNC+H_BACK+27-47)));
	asm volatile("bltz	$a0,label3_2");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label3_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label3");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label3:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

//�e�L�X�g���[�h
asm volatile("label3_2:");
	//	drawing=-1;
	asm volatile("addiu	$t1,$zero,-1");
	asm volatile("la	$v0,%0"::"i"(&drawing));
	asm volatile("sb	$t1,0($v0)");
	//	a0=VRAMp;
	asm volatile("la	$v0,%0"::"i"(&VRAMp));
	asm volatile("lw	$a0,0($v0)");
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));
	//	a2=fontp+�`�撆�̍s%8
	asm volatile("la	$v0,%0"::"i"(&LineCount));
	asm volatile("lhu	$v1,0($v0)");
//	asm volatile("la	$a2,%0"::"i"(FontData));
	asm volatile("la	$v0,%0"::"i"(&fontp));
	asm volatile("lw	$a2,0($v0)");
	asm volatile("addiu	$v1,$v1,%0"::"n"(-V_SYNC-V_PREEQ-1));
	asm volatile("andi	$v1,$v1,7");
	asm volatile("addu	$a2,$a2,$v1");
	//	v1=bgcolor�g�`�f�[�^
	asm volatile("la	$v0,%0"::"i"(&bgcolor));
	asm volatile("lw	$v1,0($v0)");
	//	v0=&LATB;
	asm volatile("la	$v0,%0"::"i"(&LATB));
	//	t6=twidth
	asm volatile("la	$t0,%0"::"i"(&twidth));
	asm volatile("lbu	$t6,0($t0)");
	//twidth��40�̎�jump4�ɃW�����v
	asm volatile("sltiu	$t0,$t6,40");
	asm volatile("nop");
	asm volatile("beq	$t0,$zero,jump4");
// ��������30�������[�h�o�͔g�`�f�[�^����
	asm volatile("lbu	$t0,0($a0)");
	asm volatile("lbu	$t1,%0($a0)"::"n"(ATTROFFSET1));
	asm volatile("sll	$t0,$t0,3");
	asm volatile("addu	$t0,$t0,$a2");
	asm volatile("lbu	$t0,0($t0)");
	asm volatile("sll	$t1,$t1,2");
	asm volatile("addu	$t1,$t1,$a1");
	//fontp��0xa0000000�ȏ�̏ꍇ(RAM�̏ꍇ)jump2�ɃW�����v
	asm volatile("lui	$t2,0xa000");
	asm volatile("sltu	$t2,$a2,$t2");
	asm volatile("nop");
	asm volatile("beq	$t2,$zero,jump2");
//	-----------------
	asm volatile("lw	$t1,0($t1)");
	asm volatile("addiu	$a0,$a0,1");//�����Ő����N���b�N���������肩��604�N���b�N

asm volatile("loop1:");
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)"); // �����ōŏ��̐M���o��
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($a0)"); //�󂢂Ă��鎞�ԂɎ��̕����o�͂̏����J�n
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET1));
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lw	$t5,0($t5)");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
					asm volatile("addiu	$t6,$t6,-1");//���[�v�J�E���^
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,1,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
			asm volatile("lbu	$t4,0($t4)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("sb	$t2,0($v0)");
	
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,0,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("addu	$t1,$zero,$t5");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("addu	$t0,$zero,$t4");
	asm volatile("sb	$t2,0($v0)");//�x���X���b�g�̂���1�N���b�N�x���
	asm volatile("bnez	$t6,loop1");

	asm volatile("b		jump1");

// �t�H���g��RAM�̏ꍇ
//	-----------------
asm volatile("jump2:");
	asm volatile("nop");//RAM�ǂݏo���̂��߂̃N���b�N����
	asm volatile("lw	$t1,0($t1)");
	asm volatile("addiu	$a0,$a0,1");//�����Ő����N���b�N���������肩��604�N���b�N
asm volatile("loop1_2:");
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)"); // �����ōŏ��̐M���o��
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($a0)"); //�󂢂Ă��鎞�ԂɎ��̕����o�͂̏����J�n
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET1));
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lw	$t5,0($t5)");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
					asm volatile("addiu	$t6,$t6,-1");//���[�v�J�E���^
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,1,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($t4)");
					asm volatile("nop");//RAM�ǂݏo���̂���1�N���b�N����
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,0,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("addu	$t1,$zero,$t5");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("addu	$t0,$zero,$t4");
	asm volatile("sb	$t2,0($v0)");//�x���X���b�g�̂���1�N���b�N�x���
	asm volatile("bnez	$t6,loop1_2");

	asm volatile("nop");
	asm volatile("b		jump1");

// ��������40�������[�h�o�͔g�`�f�[�^����
asm volatile("jump4:");
	asm volatile("lbu	$t0,0($a0)");
	asm volatile("lbu	$t1,%0($a0)"::"n"(ATTROFFSET2));
	asm volatile("sll	$t0,$t0,3");
	asm volatile("addu	$t0,$t0,$a2");
	asm volatile("lbu	$t0,0($t0)");
	asm volatile("sll	$t1,$t1,2");
	asm volatile("addu	$t1,$t1,$a1");
	//fontp��0xa0000000�ȏ�̏ꍇ(RAM�̏ꍇ)jump2_2�ɃW�����v
	asm volatile("lui	$t2,0xa000");
	asm volatile("sltu	$t2,$a2,$t2");
	asm volatile("nop");
	asm volatile("beq	$t2,$zero,jump2_2");
//	-----------------
	asm volatile("lw	$t1,0($t1)");
	asm volatile("addiu	$a0,$a0,1");//�����Ő����N���b�N���������肩��604�N���b�N

asm volatile("loop1_4:");
//������
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($a0)"); //�󂢂Ă��鎞�ԂɎ��̕����o�͂̏����J�n
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET2));
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lw	$t5,0($t5)");
			asm volatile("addiu	$t6,$t6,-2");//���[�v�J�E���^
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("rotr	$t1,$t5,16");
	asm volatile("sb	$t2,0($v0)");
			asm volatile("lbu	$t0,0($t4)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("sb	$t2,0($v0)");

//���
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,1($a0)");
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET2+1));
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
			asm volatile("addiu	$a0,$a0,2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
			asm volatile("lbu	$t4,0($t4)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("lw	$t1,0($t5)");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("addu	$t0,$zero,$t4");
	asm volatile("sb	$t2,0($v0)");//�x���X���b�g�̂���1�N���b�N�x���
	asm volatile("bnez	$t6,loop1_4");

	asm volatile("b		jump1");
	asm volatile("nop");

// �t�H���g��RAM�̏ꍇ
//	-----------------
asm volatile("jump2_2:");
	asm volatile("nop");//RAM�ǂݏo���̂��߂̃N���b�N����
	asm volatile("lw	$t1,0($t1)");
	asm volatile("addiu	$a0,$a0,1");//�����Ő����N���b�N���������肩��604�N���b�N

asm volatile("loop1_5:");
//������
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($a0)"); //�󂢂Ă��鎞�ԂɎ��̕����o�͂̏����J�n
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET2));
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lw	$t5,0($t5)");
			asm volatile("addiu	$t6,$t6,-2");//���[�v�J�E���^
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("rotr	$t1,$t5,16");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t0,0($t4)");
					asm volatile("nop");//RAM�ǂݏo���̂��߂̃N���b�N����
	asm volatile("sb	$t2,0($v0)");

//���
	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,7,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,1($a0)");
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSET2+1));
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,6,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t4,$t4,3");
			asm volatile("addu	$t4,$t4,$a2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,5,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("sll	$t5,$t5,2");
			asm volatile("addu	$t5,$t5,$a1");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,4,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
asm volatile("nop");
			asm volatile("addiu	$a0,$a0,2");
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,3,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
	asm volatile("rotr	$t1,$t1,24");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("lbu	$t4,0($t4)");
					asm volatile("nop");//RAM�ǂݏo���̂��߂̃N���b�N����
	asm volatile("sb	$t2,0($v0)");

	asm volatile("addu	$t2,$zero,$v1");
	asm volatile("ext	$t3,$t0,2,1");
	asm volatile("movn	$t2,$t1,$t3");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
	asm volatile("rotr	$v1,$v1,24");
			asm volatile("lw	$t1,0($t5)");
	asm volatile("sb	$t2,0($v0)");
	asm volatile("rotr	$t2,$t2,8");
			asm volatile("addu	$t0,$zero,$t4");
	asm volatile("sb	$t2,0($v0)");//�x���X���b�g�̂���1�N���b�N�x���
	asm volatile("bnez	$t6,loop1_5");

	asm volatile("b		jump1");
	asm volatile("nop");

//��������O���t�B�b�N���[�h�̏o��
asm volatile("jump3:");
	//TMR2�̒l�Ń^�C�~���O�̂����␳
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(G_H_SYNC+G_H_BACK+18-15)));
	asm volatile("bltz	$a0,label3_4");
	asm volatile("addiu	$v0,$a0,-10");
	asm volatile("bgtz	$v0,label3_4");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label3_3");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label3_3:");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
	asm volatile("nop");asm volatile("nop");

asm volatile("label3_4:");
	//	drawing=-1;
	asm volatile("addiu	$t1,$zero,-1");
	asm volatile("la	$v0,%0"::"i"(&drawing));
	asm volatile("sb	$t1,0($v0)");
	//	t1=VRAMp;
	asm volatile("la	$v0,%0"::"i"(&VRAMp));
	asm volatile("lw	$t1,0($v0)");
	//	v1=gClTable;
	asm volatile("la	$v1,%0"::"i"(gClTable));
	//	v0=&LATB;
	asm volatile("la	$v0,%0"::"i"(&LATB));
	//	t2=0;
	asm volatile("addiu	$t2,$zero,0");

	asm volatile("addiu	$a2,$zero,12"); //���[�v�J�E���^
	asm volatile("lhu	$a1,0($t1)");
	asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
	asm volatile("rotr	$a1,$a1,12");

	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
	asm volatile("lbu	$t0,0($a0)");
	asm volatile("rotr	$a1,$a1,28");
//	asm volatile("nop");

//20�h�b�g�~12�񃋁[�v
//-------------------------------------------------
asm volatile("loop1_3:"); //������LATB�ɍŏ��̏o�́B�����������������肩��735�T�C�N���ɂȂ�悤��������
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,1($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,2($a0)");
				asm volatile("rotr	$a1,$a1,28");
asm volatile("nop");
asm volatile("nop");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,3($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,4($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,5($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,6($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,7($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,9($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,0($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,1($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,2($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,3($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,4($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,5($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,6($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,7($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,9($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,0($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,1($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,2($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,3($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,4($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,5($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,6($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,7($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,9($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,0($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,1($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,2($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,3($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,4($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,5($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,6($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,7($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,8($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,9($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,0($a0)");
							asm volatile("addiu	$a2,$a2,-1");//���[�v�J�E���^
	asm volatile("rotr	$a1,$a1,28");
							asm volatile("bnez	$a2,loop1_3");
//-------------------------------------------------
//�c16�h�b�g
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,1($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,2($a0)");
				asm volatile("rotr	$a1,$a1,28");
asm volatile("nop");
asm volatile("nop");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,3($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,4($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,5($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,6($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,7($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,9($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,0($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,1($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,2($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,3($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,4($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,5($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,6($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,7($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,9($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,0($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,1($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,2($a0)");
							asm volatile("lhu	$a1,0($t1)");
							asm volatile("addiu	$t1,$t1,2"); //VRAMp++;
							asm volatile("rotr	$a1,$a1,12");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,3($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,4($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,5($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,6($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("rotr	$a1,$a1,28");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,7($a0)");
	asm volatile("ins	$t2,$a1,4,4");
	asm volatile("addu	$a0,$t2,$v1");
				asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,8($a0)");
asm volatile("nop");
asm volatile("nop");
	asm volatile("rotr	$a1,$a1,28");
	asm volatile("sb	$t0,0($v0)");
	asm volatile("lbu	$t0,9($a0)");
				asm volatile("ins	$t2,$a1,4,4");
				asm volatile("addu	$a0,$t2,$v1");
	asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,0($a0)");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
				asm volatile("sb	$t0,0($v0)");
				asm volatile("lbu	$t0,1($a0)");
asm volatile("nop");
asm volatile("nop");
				asm volatile("sb	$t0,0($v0)");
//-------------------------------------------------

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");


asm volatile("jump1:");

	//	LATB=C_BLK;
	asm volatile("addiu	$t0,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t0,0($v0)");
 	IFS0bits.OC2IF = 0;			// OC2���荞�݃t���O�N���A
}

// �e�L�X�g��ʃN���A
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<WIDTH_X2*WIDTH_Y*2/4;i++) *vp++=0;
	cursor=TVRAM;
}
// �O���t�B�b�N��ʃN���A
void g_clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)gVRAM;
	for(i=0;i<G_H_WORD*G_Y_RES/2;i++) *vp++=0;
}

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g)
{
	// �e�L�X�g�J���[�p���b�g�ݒ�i5�r�b�gDA�A�d��3.3V�A90�x�P�ʁj
	// n:�p���b�g�ԍ��Ar,g,b:0�`255
	int y,s0,s1,s2,s3;
	y=(150*g+29*b+77*r+128)/256;
	s0=(3525*y+3093*((int)r-y)+1440*256+32768)/65536;
	s1=(3525*y+1735*((int)b-y)+1440*256+32768)/65536;
	s2=(3525*y-3093*((int)r-y)+1440*256+32768)/65536;
	s3=(3525*y-1735*((int)b-y)+1440*256+32768)/65536;
	ClTable[n]=s0+(s1<<8)+(s2<<16)+(s3<<24);
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	// �e�L�X�g�o�b�N�O�����h�J���[�ݒ� r,g,b:0�`255
	int y,s0,s1,s2,s3;
	y=(150*g+29*b+77*r+128)/256;
	s0=(3525*y+3093*((int)r-y)+1440*256+32768)/65536;
	s1=(3525*y+1735*((int)b-y)+1440*256+32768)/65536;
	s2=(3525*y-3093*((int)r-y)+1440*256+32768)/65536;
	s3=(3525*y-1735*((int)b-y)+1440*256+32768)/65536;
	bgcolor=s0+(s1<<8)+(s2<<16)+(s3<<24);
}

void g_set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g)
{
	// �O���t�B�b�N�J���[�p���b�g�ݒ�i5�r�b�gDA�A�d��3.3V�A10�h�b�g��1���̃p�^�[���j
	// n:�p���b�g�ԍ��Ar,g,b:0�`255
	// �P�xY=0.587*G+0.114*B+0.299*R
	// �M��N=Y+0.4921*(B-Y)*sin��+0.8773*(R-Y)*cos��
	// �o�̓f�[�^S=(N*0.71[v]+0.29[v])/3.3[v]*64

	int y;
	if(n>=16) return;
	y=(150*g+29*b+77*r+128)/256;
	gClTable[n*16+0]=(3525*y+   0*((int)b-y)+3093*((int)r-y)+1440*256+32768)/65536;//��=2��*0/15
	gClTable[n*16+1]=(3525*y+1725*((int)b-y)- 323*((int)r-y)+1440*256+32768)/65536;//��=2��*4/15
	gClTable[n*16+2]=(3525*y-1020*((int)b-y)-2502*((int)r-y)+1440*256+32768)/65536;//��=2��*9/15
	gClTable[n*16+3]=(3525*y-1289*((int)b-y)+2069*((int)r-y)+1440*256+32768)/65536;//��=2��*13/15
	gClTable[n*16+4]=(3525*y+1650*((int)b-y)+ 956*((int)r-y)+1440*256+32768)/65536;//��=2��*3/15
	gClTable[n*16+5]=(3525*y+ 361*((int)b-y)-3025*((int)r-y)+1440*256+32768)/65536;//��=2��*7/15
	gClTable[n*16+6]=(3525*y-1650*((int)b-y)+ 956*((int)r-y)+1440*256+32768)/65536;//��=2��*12/15
	gClTable[n*16+7]=(3525*y+ 706*((int)b-y)+2825*((int)r-y)+1440*256+32768)/65536;//��=2��*1/15
	gClTable[n*16+8]=(3525*y+1020*((int)b-y)-2502*((int)r-y)+1440*256+32768)/65536;//��=2��*6/15
	gClTable[n*16+9]=(3525*y-1503*((int)b-y)-1546*((int)r-y)+1440*256+32768)/65536;//��=2��*10/15
}

void start_composite(void)
{
	// �ϐ������ݒ�
	LineCount=0;				// ���������C���J�E���^�[
	drawing=0;
	if(!graphmode){
		VRAMp=TVRAM;
		PR2 = H_NTSC -1;		// ��63.5usec�ɐݒ�
	}
	else{
		VRAMp=(unsigned char *)gVRAM;
		PR2 = G_H_NTSC -1; 		// ��63.5usec�ɐݒ�
	}
	TMR2=0;
	T2CONSET=0x8000;			// �^�C�}2�X�^�[�g
}
void stop_composite(void)
{
	T2CONCLR = 0x8000;			// �^�C�}2��~
	OC1CONCLR = 0x8000;			// OC1��~
	OC2CONCLR = 0x8000;			// OC2��~
	OC3CONCLR = 0x8000;			// OC3��~
}

// �J���[�R���|�W�b�g�o�͏�����
void init_composite(void)
{
	int i;
	graphmode=0;//�e�L�X�g���[�h
	twidth=WIDTH_X1;//30�������[�h
	clearscreen();

	fontp=(unsigned char *)FontData;
	//�e�L�X�g�p�J���[�ԍ�0�`7�̃p���b�g������
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(;i<256;i++){
		set_palette(i,255,255,255); //8�ȏ�͑S�Ĕ��ɏ�����
	}
	set_bgcolor(0,0,0); //�o�b�N�O�����h�J���[�͍�
	setcursorcolor(7);

	// �^�C�}2�̏����ݒ�,�����N���b�N��63.5usec�����A1:1
	T2CON = 0x0000;				// �^�C�}2��~���
	IPC2bits.T2IP = 5;			// ���荞�݃��x��5
	IFS0bits.T2IF = 0;
	IEC0bits.T2IE = 1;			// �^�C�}2���荞�ݗL����

	// OC1�̊��荞�ݗL����
	IPC1bits.OC1IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC1IF = 0;
	IEC0bits.OC1IE = 1;			// OC1���荞�ݗL����

	// OC2�̊��荞�ݗL����
	IPC2bits.OC2IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC2IF = 0;
	IEC0bits.OC2IE = 1;			// OC2���荞�ݗL����

	// OC3�̊��荞�ݗL����
	IPC3bits.OC3IP = 5;			// ���荞�݃��x��5
	IFS0bits.OC3IF = 0;
	IEC0bits.OC3IE = 1;			// OC3���荞�ݗL����

	OSCCONCLR=0x10; // WAIT���߂̓A�C�h�����[�h
	BMXCONCLR=0x40;	// RAM�A�N�Z�X�E�F�C�g0
	INTEnableSystemMultiVectoredInt();
	start_composite();
}

//30�������[�h(8�h�b�g�t�H���g)��40�������[�h(6�h�b�g�t�H���g)�̐؂�ւ�
void set_width(unsigned char m){
// m:0�@30�������[�h�A1�@40�������[�h
// �O���t���[�h���͖���
// PCG�g�p���̓t�H���g�ύX���Ȃ�
	if(graphmode) return;
	clearscreen();
	if(m){
		if(fontp<(unsigned char *)0xa0000000) fontp=(unsigned char *)FontData2;
		twidth=WIDTH_X2;
	}
	else{
		if(fontp<(unsigned char *)0xa0000000) fontp=(unsigned char *)FontData;
		twidth=WIDTH_X1;
	}
	return;
}

//�e�L�X�g���[�h�ƃO���t�B�b�N���[�h�̐؂�ւ�
void set_graphmode(unsigned char m){
// m:0�@�e�L�X�g���[�h�A0�ȊO �O���t�B�b�N���[�h
	stop_composite();
	if(m){
		//�O���t�B�b�N���[�h�J�n
		OSCConfig(OSC_POSC_PLL, OSC_PLL_MULT_15, OSC_PLL_POST_1, 0); //�V�X�e���N���b�N��PLL��15�{�ɐݒ�
		graphmode=1;
	}
	else{
		//�e�L�X�g�[���[�h�J�n
		OSCConfig(OSC_POSC_PLL, OSC_PLL_MULT_16, OSC_PLL_POST_1, 0); //�V�X�e���N���b�N��PLL��16�{�ɐݒ�
		graphmode=0;
	}
	start_composite();
}
void init_graphic(unsigned short *gvram){
// �O���t�B�b�N�@�\��������
// �p�����[�^�Ƃ��āA28KB�̃������̈�ւ̃|�C���^��^����
	int i;
	gVRAM=gvram;
	g_clearscreen();

	//�O���t�B�b�N�p�J���[�p���b�g������
	for(i=0;i<8;i++){
		g_set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		g_set_palette(8+i,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
}