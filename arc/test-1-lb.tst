#----------------------------------------------------------------------
#
#		qvi 自動テストスクリプト
#
#		テストスクリプト仕様：
#			http://vivi.dyndns.org/vivi/docs/qvi/qvi-029.html
#
#----------------------------------------------------------------------

#	テストスクリプト要件：
#		[1] 全てのコマンドを網羅しているか？
#		[2] {c|d|y}<move> の全てのコマンドを網羅しているか？
#		[3] 繰り返し回数を前置した場合もチェック
#		[4] 編集コマンド実行後のカーソル位置は正しいか？
#		[5] 編集コマンド実行後の .(repeat the command) は正しく動作するか？
#		[6] .(repeat the command) に繰り返し回数を前置した場合も大丈夫か？
#		[7] 編集コマンド実行後の u(undo)/U(redo) は正しく動作するか？
#		[8] u(undo)/U(redo) 実行後の カーソル位置は正しいか？


:set fontName=Courier New
:set fontSize=14
:set linebreak
##----------------------------------------------------------------------
##	h j k l による単純なカーソル移動
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<}
=1 0
>main()
>{
>	printf("hello, world.\n");
>}
>
"j" =2 0
"j" =3 0
"j" =4 0
"j" =5 0	#	EOF行
"j" =5 0	#	EOF行
"k" =4 0
"k" =3 0
"k" =2 0
"k" =1 0
"k" =1 0
"l" =1 1	#	a
"l" =1 2	#	i
"l" =1 3	#	n
"l" =1 4	#	(
"l" =1 5	#	)
"l" =1 5
"h" =1 4
"h" =1 3
"h" =1 2
"h" =1 1
"h" =1 0
"h" =1 0

<{
<	hoge;
<}
"$j" =2 5
"j" =3 0
"k" =2 5

##---------------------------------------------------------------------
##	h j k l 繰り返し回数前置
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<}
"2j" =3 0
"3j" =5 0
"3k" =2 0
"3k" =1 0
"3l" =1 3
"3l" =1 5
"3h" =1 2
"3h" =1 0

##---------------------------------------------------------------------
##	j k でカラム位置が保存されるかどうか
##---------------------------------------------------------------------
<012345
<012
<0123456789
"5l" =1 5
"j"  =2 2
"j"  =3 5

##---------------------------------------------------------------------
##	w b e
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<}
=1 0
"w" =1 4	#	(
"w" =2 0	#	{
"w" =3 1	#	print
"w" =3 7	#	("
"w" =3 9	#	hello
"w" =3 14	#	,
"w" =3 16	#	world
"w" =3 21	#	.\
"w" =3 23	#	n
"w" =3 24	#	");
"w" =4 1	#	123xy
"w" =4 7	#	xyz
"w" =4 10	#	あい
"w" =4 12	#	漢字
"w" =4 14	#	あい
"w" =4 16	#	カナ
"w" =4 18	#	漢字
"w" =4 20	#	あい
"w" =4 22	#	。
"w" =4 23	#	漢字
"w" =4 25	#	。
"w" =5 0	#	}
"w" =6 0	#	EOF
"w" =6 0	#	EOF
"b" =5 0	#	}
"b" =4 25	#	。
"b" =4 23	#	漢字
"b" =4 22	#	。
"b" =4 20	#	あい
"b" =4 18	#	漢字
"b" =4 16	#	カナ
"b" =4 14	#	あい
"b" =4 12	#	漢字
"b" =4 10	#	あい
"b" =4 7	#	xyz
"b" =4 1	#	123xy
"b" =3 24	#	");
"b" =3 23	#	n
"b" =3 21	#	.\
"b" =3 16	#	world
"b" =3 14	#	,
"b" =3 9	#	hello
"b" =3 7	#	("
"b" =3 1	#	print
"b" =2 0	#	{
"b" =1 4	#	(
"b" =1 0	#	main
"b" =1 0	#	main
#	main()
#	{
#		printf("hello, world.\n");
#		123xy xyzあい漢字あいカナ漢字あい。漢字。
#	}
"e" =1 3	#	main
"e" =1 5	#	)
"e" =2 0	#	{
"e" =3 6	#	printf
"e" =3 8	#	("
"e" =3 13	#	hello
"e" =3 14	#	,
"e" =3 20	#	world
"e" =3 22	#	.\
"e" =3 23	#	n
"e" =3 26	#	");
"e" =4 5	#	123xy
"e" =4 9	#	xyz
"e" =4 11	#	あい
"e" =4 13	#	漢字
"e" =4 15	#	あい
"e" =4 17	#	カナ
"e" =4 19	#	漢字
"e" =4 21	#	あい
"e" =4 22	#	。
"e" =4 24	#	漢字
"e" =4 25	#	。
"e" =5 0	#	}
"e" =6 0	#	EOF
"e" =6 0	#	EOF

<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<}
=1 0
"2w" =2 0	#	{
"99w" =6 0
"2b" =4 25	#	。
"99b" =1 0
"3e" =2 0	#	{
"99e" =6 0	#	EOF

##---------------------------------------------------------------------
##	W B E
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<
<}
=1 0
"W" =2 0	#  {
"W" =3 1	#  print
"W" =3 16	#  world
"W" =4 1	#  123
"W" =4 7	#  xyz
"W" =6 0	#  }
"W" =7 0	#  EOF
"B" =6 0	#  }
"B" =4 7	#  xyz
"B" =4 1	#  123
"B" =3 16	#  world
"B" =3 1	#  print
"B" =2 0	#  {
"B" =1 0	#  main
"E" =1 5	#  )
"E" =2 0	#  {
"E" =3 14	#  ,
"E" =3 26	#  ;
"E" =4 5	#  y
"E" =4 25	#  。
"E" =6 0	#  }
"E" =7 0	#  EOF

<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<}
=1 0
"2W" =3 1	#	{
"99W" =6 0
"2B" =4 7	#	xyz
"99B" =1 0
"3E" =3 14	#	,
"99E" =6 0	#	EOF
##---------------------------------------------------------------------
##	0 ^ $ 行内移動コマンド
##---------------------------------------------------------------------
<	abc
=1 0
"$" =1 3
"^" =1 1
"0" =1 0
<あいう
"$" =1 2
"^" =1 0
"0" =1 0

##---------------------------------------------------------------------
##	% コマンド
##---------------------------------------------------------------------
<main()
<{
<	for(;;) {
<	}
<}
"%" =1 5
"%" =1 4
"%" =1 5
"2G%" =5 0
"%" =2 0
"3G%" =3 7
"%" =3 4
"%" =3 7
"0f;%" =3 4
"gg2%" =1 5		# 繰り返し回数は無視

##---------------------------------------------------------------------
##	} { コマンド
##---------------------------------------------------------------------
<main()
<
<hoge
<fuga
<
<foo
"}" =2 0
"}" =5 0
"}" =7 0
"{" =5 0
"{" =2 0
"{" =1 0
"2}" =5 0
"2}" =7 0
"2{" =2 0
"2{" =1 0

##---------------------------------------------------------------------
##	]] [[ コマンド
##---------------------------------------------------------------------
<main()
<{
<}
<foo()
<{
<}
<bar()
<{
<}
"]]" =2 0
"]]" =5 0
"]]" =8 0
"]]" =10 0
"[[" =8 0
"[[" =5 0
"[[" =2 0
"[[" =1 0
"3]]" =8 0
"3]]" =10 0
"2[[" =5 0
"3[[" =1 0

##---------------------------------------------------------------------
##	G gg コマンド
##---------------------------------------------------------------------
<	main()
<{
<	print;
<}
<foo()
<{
<}
<bar()
<{
<	}
"G" =10 1
"gg" =1 1
"2G" =2 0
"3G" =3 1
<hoge
<fuga
"2GJ"	//	EOF行を空でないようにする
>hoge
>fuga 
"ggG" =2 0

##---------------------------------------------------------------------
##	- + Enter コマンド
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<}
"+" =2 0
"+" =3 1
"2+" =5 0
"2+" =6 0
"-" =5 0
"-" =4 1
"2-" =2 0
"3-" =1 0
0x0d =2 0
0x0d =3 1
"2" 0x0d =5 0
"2" 0x0d =6 0

##---------------------------------------------------------------------
##	H M L コマンド
##---------------------------------------------------------------------
<	1
<2
<	3
<4
<	5
"L" =5 1
"H" =1 1
"M" =3 1
"2H" =2 0
"3L" =3 1

##---------------------------------------------------------------------
##	/? n N コマンド
##---------------------------------------------------------------------
<main()
<{
<	printf("hello, world.\n");
<	123xy xyzあい漢字あいカナ漢字あい。漢字。
<}
/i
=1 2
"n" =3 3
"n" =1 2
"1G2n" =3 3

##---------------------------------------------------------------------
##	f F t T ; , コマンド
##---------------------------------------------------------------------
<mainmain
"fi" =1 2
"fi" =1 6
"fi" =1 6
"," =1 2
"0;" =1 2
"02;" =1 6
"$2," =1 2
"0ti" =1 1
"ti" =1 1
"02ti" =1 5
"," =1 3
";" =1 5
"2;" =1 5
"$Fi" =1 6
"Fi" =1 2
"Fi" =1 2
"$2;" =1 2
"$Ti" =1 7
"Ti" =1 7
"$2Ti" =1 3
"," =1 5
";" =1 3
"2;" =1 3

##---------------------------------------------------------------------
##	i コマンド
##---------------------------------------------------------------------
<hoge
"ifoo" 0x1b
>foohoge
>
=1 2	#	Esc でカーソルは左にひとつ戻る
"."		#	repeat i command
>fofooohoge
>
"."		#	repeat i command
>fofofoooohoge
>
"3ix" 0x1b	# ３回 x 挿入
>fofofoxxxooohoge
>
"0iX	Y" 0x1b
>X	Yfofofoxxxooohoge
>
"u0."
>X	Yfofofoxxxooohoge
>

##---------------------------------------------------------------------
##	I コマンド
##---------------------------------------------------------------------
<	hoge
=1 0
"Ixyz" 0x1b		# hoge の直前に挿入
>	xyzhoge
>
"$4I8" 0x1b		# 8 を 4回挿入
>	8888xyzhoge
>
"u"
>	xyzhoge
>
"."		# repeat I command
>	8888xyzhoge
>
"u3."	# 8を4回ではなく3回挿入
>	888xyzhoge
>
##---------------------------------------------------------------------
##	a コマンド
##---------------------------------------------------------------------
<hoge
"afoo" 0x1b
>hfoooge
>
=1 3	#	Esc でカーソルは左にひとつ戻る
"."		#	repeat i command
>hfoofoooge
>
"."		#	repeat i command
>hfoofoofoooge
>
"3ax" 0x1b	# ３回 x 挿入
>hfoofoofooxxxoge
>
##---------------------------------------------------------------------
##	A コマンド
##---------------------------------------------------------------------
<	hoge
=1 0
"Axyz" 0x1b		# hoge の直前に挿入
>	hogexyz
>
"04A8" 0x1b		# 8 を 4回挿入
>	hogexyz8888
>
"u"		# xyz と 8888 は一度に undo される
>	hoge
>
"."		# repeat A command
>	hoge8888
>
"u3."	# 8を4回ではなく3回挿入
>	hoge888
>
##---------------------------------------------------------------------
##	C コマンド
##---------------------------------------------------------------------
<main()
"fiCXYZ" 0x1b
>maXYZ
>
=1 4

##---------------------------------------------------------------------
##	cc コマンド
##---------------------------------------------------------------------
<abc
<xyz
<123
"2Gcchogehoge" 0x1b
>abc
>hogehoge
>123
>
=2 7
"u1G2ccfoo" 0x1b	#	1, 2 行を置換
>foo
>123
>

##---------------------------------------------------------------------
##	S コマンド	（オートインデント非サポート）
##---------------------------------------------------------------------
<abc
<xyz
<123
"2GShogehoge" 0x1b
>abc
>hogehoge
>123
>
=2 7
"u1G2Sfoo" 0x1b	#	1, 2 行を置換
>foo
>123
>
##---------------------------------------------------------------------
##	s コマンド
##---------------------------------------------------------------------
<main
"sXYZ" 0x1b
=1 2
>XYZain
>
"$."	# n に移動して再置換
>XYZaiXYZ
>
"03s54321" 0x1b		# 3文字→5文字置換
>54321aiXYZ
>
"fX2."		# X に移動して 2文字→5文字置換
>54321ai54321Z
>
##---------------------------------------------------------------------
##	o コマンド
##---------------------------------------------------------------------
<hoge
"oabc" 0x1b
>hoge
>abc
>
=2 2
"."
>hoge
>abc
>abc
>
"1G3ox" 0x1b	//	x<改行> を３行挿入
>hoge
>x
>x
>x
>abc
>abc
>
##---------------------------------------------------------------------
##	O コマンド
##---------------------------------------------------------------------
<hoge
"Oabc" 0x1b
>abc
>hoge
>
=1 2
"3OZZZ" 0x1b
>ZZZ
>ZZZ
>ZZZ
>abc
>hoge
>
##---------------------------------------------------------------------
##	x X コマンド
##---------------------------------------------------------------------
<hogefuga
"x"
>ogefuga
>
"u"
=1 0	### undo 後のカーソル位置は削除した文字の先頭
"03x"
>efuga
>
"u0."
>efuga
>
"u02."
>gefuga
>
"u0ff9x"
>hoge
>
"u0X"
>hogefuga
>
"ffX"
>hogfuga
>
"u" =1 4	## 削除前カーソル位置
0ff3X"
>hfuga
>
##---------------------------------------------------------------------
##	D コマンド
##---------------------------------------------------------------------
<main()
"fiD"
=1 1
>ma
>
"u0fi2D"	# 繰り返し回数は無視される
=1 1
>ma
>
"u0fn."
>mai
>


##---------------------------------------------------------------------
##	dd, d<move> コマンド
##---------------------------------------------------------------------
<1
<2
<3
<4
<5
"2Gdd"	#  ２行目を削除
>1
>3
>4
>5
>
"u"
>1
>2
>3
>4
>5
>
=2 0	#  カーソルは2行目に移動
0x1b "2."	#  ２行削除
>1
>4
>5
>
"u"
>1
>2
>3
>4
>5
>
=2 0	#  カーソルは2行目
<abc =  1;
"dw"	# 次の単語先頭直前まで削除
>=  1;
>
"u0."	# undo & 行頭移動 & repeat dw コマンド
>=  1;
>
"u02."	# undo & 行頭移動 & repeat dw コマンド 2-times
>1;
>
"u0d2w"	# undo & 行頭移動 & d2w
>1;
>
"u0."	# undo & 行頭移動 & repeat d2w コマンド
>1;
>
<abc = 1;
<xyz = 123;
"f;dw"	#  行末の単語（;）を削除
>abc = 1
>xyz = 123;
>
"u0f;d2w"	#  行末から次の行の最初の単語を削除
>abc = 1= 123;
>

<abc = 1;
"de"		# abc のみを削除
> = 1;
>

<main()
<
<hoge
"f(" =1 4
"d}"		#  ２行目までを行単位削除
>hoge
>

<main()
"cfn123" 0x1b
>123()
>
"u0l."
>m123()
>

<main()
"dfi"	# i まで削除
>n()
>
"u0d;"	# i まで削除
>n()
>
"u0dti"	# i 直前まで削除
>in()
>

##---------------------------------------------------------------------
##	r コマンド
##---------------------------------------------------------------------
<main()
"r3"
>3ain()
>
"u03r0"
>000n()
>
<hoge
<fuga
"3r="
>===e
>fuga
>
"u0fg3r="
>ho===
>fuga
>

##---------------------------------------------------------------------
##	R コマンド
##---------------------------------------------------------------------

<main()
"RXY" 0x1b =1 1
>XYin()
>
"u03R=" 0x1b =1 2
>===ain()
>
<hoge
<fuga
"R=======" 0x1b		# 改行は上書きされない
>=======
>fuga
>

##---------------------------------------------------------------------
##	dd d<move> x X 後の p P
##---------------------------------------------------------------------
<main()
"xp"	#  m, a を入れ替え
>amin()
>
"2p"	# m を２回ペースト
>ammmin()
>
<main()
<hoge
"lddp"	#  2文字目に移動して、1, 2 行目を入れ替え
>hoge
>main()
>
<11
<222
<3333
<44444
"jl2YP"	# 2行目２文字目に移動してヤンク・ペースト
=2 0	# カーソルは２行目最初
>11
>222
>3333
>222
>3333
>44444
>
"ulp"	# 元にもどして、2行目２文字目に移動してペースト
>11
>222
>222
>3333
>3333
>44444
>
=3 0
"2yyP"
>11
>222
>222
>3333
>222
>3333
>3333
>44444
>
#	空でないEOF行で行単位pした場合
<abc
<xyzzz
"3GiZZZ" 0x1b	#	空でないEOF行作成
>abc
>xyzzz
>ZZZ
"kdd"	#	２行目削除
>abc
>ZZZ
=2 0
"p"		#	行単位ペースト
>abc
>ZZZ
>xyzzz
>
"2p"	#	2行ペースト
>abc
>ZZZ
>xyzzz
>xyzzz
>xyzzz
>

##---------------------------------------------------------------------
##	yy y<move> Y 後の p P
##---------------------------------------------------------------------
<main()
"ywP"
=1 0
>mainmain()
>
"uywp"
>mmainain()
>
"ufiYp"
=2 0
>main()
>main()
>
<foo
<bar
<hoge
"yyGp"
>foo
>bar
>hoge
>foo
>
<main()
"yfiP"
>maimain()
>
"y;p"
>mmaiaimain()
>

##---------------------------------------------------------------------
##	J コマンド
##---------------------------------------------------------------------
<a
<x
"J"		# join 時に空白が挿入される
>a x
>
<a   
<x
"J"		# join 時、行末に空白類がある場合は空白は挿入されない
>a   x
>
<a
<		x
"J"		# join 時、次行先頭空白類列は空白ひとつに置換される
>a x
>
<a
<b
<c
<d
"3J"	# 3回join ではなく、3行join
>a b c
>d
>
"u"
>a
>b
>c
>d
>
"2G."
>a
>b c d
>

##---------------------------------------------------------------------
##	>> << コマンド
##---------------------------------------------------------------------
<hoge
<	fuga
<	foo
<bar
"2>>"
>	hoge
>		fuga
>	foo
>bar
>
"u"
>hoge
>	fuga
>	foo
>bar
>
"2<<"
>hoge
>fuga
>	foo
>bar
>
"u"
>hoge
>	fuga
>	foo
>bar
>
"3G."
>hoge
>	fuga
>foo
>bar
>

##---------------------------------------------------------------------
##	:<digits> Enter
##---------------------------------------------------------------------
<main() {
<	hoge;
<}
=1 0
:2
=2 1
:3
=3 0
:4
=4 0	# EOF 行
:1
=1 0
:5
=1 0	# EOF 行を超えた場合は移動しない
:1,2
=2 1	# 行番号を複数指定した場合は、最後の行番号が有効
:0
=1 0	# :0 は先頭行に移動
:1+2
=3 0
:.+2
=3 0	# ２行進む
:4
:.-2
=2 1	# ２行戻る
:1
:+2
=3 0	# ２行進む
:4
:-2
=2 1	# ２行戻る
:$
=3 0
##---------------------------------------------------------------------
##	:<range>d
##---------------------------------------------------------------------
<1
<2
<3
<4
<5
:2d
>1
>3
>4
>5
>
"u"
>1
>2
>3
>4
>5
>
:2,4d
>1
>5
>
"u"
:%d
>
"u"
##---------------------------------------------------------------------
##	:<range>s/before/after/[g]
##---------------------------------------------------------------------
<1
<22
<3
:%s/2/xyz
>1
>xyz2
>3
>
"u"
:%s/2/xyz/
>1
>xyz2
>3
>
"u"
:%s/2/xyz/g
>1
>xyzxyz
>3
>

