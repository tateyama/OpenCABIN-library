OpenCABIN library version 1.0

���ʃf�B�X�v���C�p�A�v���P�[�V�����v���O�����J����Ճ\�t�g�E�F�A
==================================================
* �R���p�C���ɂ���
** �����: linux
** �R���p�C���ɐ旧���CMakefile.linux ���� GLUTDIR ���e���ɍ��킹�Ă�������
** glut �́Cfreeglut-2.4.0 �𐄏����܂�
** ���� Redhat �n�̕W���� glut �́u�񐄏��v�ł�

1) cd util; make
2) cd ..; make
==================================================
* �T���v���v���O�����̎��s�ɂ���

�@make ����� master, renderer, sample.so ���ł��܂��Dsample.so �̓T��
�v���v���O�����ł��D���̎��s���@�́C

1) �܂��^�[�~�i����2�N�����Ă���
2) 1�̃^�[�~�i���� ./renderer �����s����
3) ����1�̃^�[�~�i���� ./master -l sample.so �����s����D
4) master �����s����ƃv�����v�g���o��̂ŁC�urun�v �R�}���h����͂���

�ł��D�E�B���h�E����J���C�e�B�[�|�b�g�����l�q���ώ@�ł��܂��D����ɁC

1) �܂��^�[�~�i����3�N�����Ă���
2) 1�̃^�[�~�i���� ./renderer -p 28980 �����s����
3) ����1�̃^�[�~�i���� ./renderer -p 28981 �����s����
4) ����1�̃^�[�~�i���� ./master -l sample.so -r localhost:28980 -r localhost:28981 �����s����D
5) master �����s����ƃv�����v�g���o��̂ŁC�urun�v ����͂���

�����s���Ă݂�ƁC�E�B���h�E���Q�J���C�e�B�[�|�b�g���������ĉ��l�q
���ώ@�ł��܂��D

==================================================
* �A�v���P�[�V�����v���O�����̍쐬���@

OpenCABIN library �ł̃A�v���P�[�V�����v���O�����͊�{�I�ɁC�v���O�C��
�����ɂȂ�Cmaster, renderer �ɂ����s����܂��Dsample_tmpl.c �����`��
�Ȃ�܂��Dminit()�Cmidle(), mcleanup(), ginit(), gdraw(), gidle(),
rinit() ���������Ă��������Dm �Ŏn�܂�֐��� master ����Ă΂�܂��D g
���邢�� r �Ŏn�܂�֐��� renderer ����Ă΂�܂��D���S�ɓ����\�[�X�R�[
�h�� sample.c �ɂȂ�܂��Dinit() �͎��s���ɍŏ��Ɉ�񂾂��Ă΂��֐���
���Didle �́C���s���Ԓ��́C�����Ăяo���ꑱ���܂��Dgdraw() ���[�`���́C
�`��̕K�v�����鎞�ɌĂяo����܂��D

==================================================
* ���L�ϐ��ɂ���

  master �ŕϐ����Z�b�g(���)���Crenderer �ł͕ϐ���ǂݏo�������ɂ���
���D�܂��C���ϐ���錾���܂����C�Ⴆ�� float �^�� g_angle �Ƃ����ϐ�
�ł���΁C

float g_angle;

�ł͂Ȃ��C

SHVARfloat(g_angle);

�Ƃ��Ă��������D�܂��C�l�̏������� minit() �̒��ŁC

  g_angle = 0.0f;
  SHVARINITfloat(g_angle);

�Ƃ��Ă��������D�l�̑���� midle() ���ŁC

  g_angle += 2.0f;

�Ƃ��Cmidle() �̍Ō�ŁC

  SHVARUPDATE(g_angle);

�ƁC�X�V��������悤�ɂ��Ă��������D����� renderer ���ł́Cgidle() ���ŁC
  SHVARREFfloat(g_angle);
�Ƃ��Cg_angle �̍X�V���󂯎���Ă��������D�l�ɍX�V������΁C
  oclPostRedisplay();
���Ăяo���C�`����X�V���Ă��������D



-- ���R �`�S 2006/4/2 (Mon)
