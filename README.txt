OpenCABIN library version 1.0

大画面ディスプレイ用アプリケーションプログラム開発基盤ソフトウェア
==================================================
* コンパイルについて
** 動作環境: linux
** コンパイルに先立ち，Makefile.linux 中の GLUTDIR を各環境に合わせてください
** glut は，freeglut-2.4.0 を推奨します
** 特に Redhat 系の標準の glut は「非推奨」です

1) cd util; make
2) cd ..; make
==================================================
* サンプルプログラムの実行について

　make すると master, renderer, sample.so ができます．sample.so はサン
プルプログラムです．その実行方法は，

1) まずターミナルを2つ起動しておく
2) 1つのターミナルで ./renderer を実行する
3) もう1つのターミナルで ./master -l sample.so を実行する．
4) master を実行するとプロンプトが出るので，「run」 コマンドを入力する

です．ウィンドウが一つ開き，ティーポットが回る様子が観察できます．さらに，

1) まずターミナルを3つ起動しておく
2) 1つのターミナルで ./renderer -p 28980 を実行する
3) もう1つのターミナルで ./renderer -p 28981 を実行する
4) もう1つのターミナルで ./master -l sample.so -r localhost:28980 -r localhost:28981 を実行する．
5) master を実行するとプロンプトが出るので，「run」 を入力する

を実行してみると，ウィンドウが２つ開き，ティーポットが同期して回る様子
が観察できます．

==================================================
* アプリケーションプログラムの作成方法

OpenCABIN library でのアプリケーションプログラムは基本的に，プラグイン
方式になり，master, renderer により実行されます．sample_tmpl.c が雛形に
なります．minit()，midle(), mcleanup(), ginit(), gdraw(), gidle(),
rinit() を実装してください．m で始まる関数は master から呼ばれます． g
あるいは r で始まる関数は renderer から呼ばれます．完全に動くソースコー
ドが sample.c になります．init() は実行時に最初に一回だけ呼ばれる関数で
す．idle は，実行期間中は，いつも呼び出され続けます．gdraw() ルーチンは，
描画の必要がある時に呼び出されます．

==================================================
* 共有変数について

  master で変数をセット(代入)し，renderer では変数を読み出すだけにしま
す．まず，大域変数を宣言しますが，例えば float 型の g_angle という変数
であれば，

float g_angle;

ではなく，

SHVARfloat(g_angle);

としてください．また，値の初期化を minit() の中で，

  g_angle = 0.0f;
  SHVARINITfloat(g_angle);

としてください．値の代入は midle() 中で，

  g_angle += 2.0f;

とし，midle() の最後で，

  SHVARUPDATE(g_angle);

と，更新をかけるようにしてください．さらに renderer 側では，gidle() 中で，
  SHVARREFfloat(g_angle);
とやり，g_angle の更新を受け取ってください．値に更新があれば，
  oclPostRedisplay();
を呼び出し，描画を更新してください．



-- 立山 義祐 2006/4/2 (Mon)
