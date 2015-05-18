/**
	@File:			interrupts.c
	@Author:		Ihsoh
	@Date:			2015-03-21
	@Description:
		中断程序。
*/

#include "interrupts.h"
#include "types.h"
#include "386.h"

static int32 unimpl_intn = 0;

#define	UNIMPL_INT(__n)	\
static	\
void	\
unimpl_int##__n(void)	\
{	\
	unimpl_intn = __n;	\
	asm volatile ("ljmp	$219, $0;");	\
	while(1);	\
}

UNIMPL_INT(0)
UNIMPL_INT(1)
UNIMPL_INT(2)
UNIMPL_INT(3)
UNIMPL_INT(4)
UNIMPL_INT(5)
UNIMPL_INT(6)
UNIMPL_INT(7)
UNIMPL_INT(8)
UNIMPL_INT(9)
UNIMPL_INT(10)
UNIMPL_INT(11)
UNIMPL_INT(12)
UNIMPL_INT(13)
UNIMPL_INT(14)
UNIMPL_INT(15)
UNIMPL_INT(16)
UNIMPL_INT(17)
UNIMPL_INT(18)
UNIMPL_INT(19)
UNIMPL_INT(20)
UNIMPL_INT(21)
UNIMPL_INT(22)
UNIMPL_INT(23)
UNIMPL_INT(24)
UNIMPL_INT(25)
UNIMPL_INT(26)
UNIMPL_INT(27)
UNIMPL_INT(28)
UNIMPL_INT(29)
UNIMPL_INT(30)
UNIMPL_INT(31)
UNIMPL_INT(32)
UNIMPL_INT(33)
UNIMPL_INT(34)
UNIMPL_INT(35)
UNIMPL_INT(36)
UNIMPL_INT(37)
UNIMPL_INT(38)
UNIMPL_INT(39)
UNIMPL_INT(40)
UNIMPL_INT(41)
UNIMPL_INT(42)
UNIMPL_INT(43)
UNIMPL_INT(44)
UNIMPL_INT(45)
UNIMPL_INT(46)
UNIMPL_INT(47)
UNIMPL_INT(48)
UNIMPL_INT(49)
UNIMPL_INT(50)
UNIMPL_INT(51)
UNIMPL_INT(52)
UNIMPL_INT(53)
UNIMPL_INT(54)
UNIMPL_INT(55)
UNIMPL_INT(56)
UNIMPL_INT(57)
UNIMPL_INT(58)
UNIMPL_INT(59)
UNIMPL_INT(60)
UNIMPL_INT(61)
UNIMPL_INT(62)
UNIMPL_INT(63)
UNIMPL_INT(64)
UNIMPL_INT(65)
UNIMPL_INT(66)
UNIMPL_INT(67)
UNIMPL_INT(68)
UNIMPL_INT(69)
UNIMPL_INT(70)
UNIMPL_INT(71)
UNIMPL_INT(72)
UNIMPL_INT(73)
UNIMPL_INT(74)
UNIMPL_INT(75)
UNIMPL_INT(76)
UNIMPL_INT(77)
UNIMPL_INT(78)
UNIMPL_INT(79)
UNIMPL_INT(80)
UNIMPL_INT(81)
UNIMPL_INT(82)
UNIMPL_INT(83)
UNIMPL_INT(84)
UNIMPL_INT(85)
UNIMPL_INT(86)
UNIMPL_INT(87)
UNIMPL_INT(88)
UNIMPL_INT(89)
UNIMPL_INT(90)
UNIMPL_INT(91)
UNIMPL_INT(92)
UNIMPL_INT(93)
UNIMPL_INT(94)
UNIMPL_INT(95)
UNIMPL_INT(96)
UNIMPL_INT(97)
UNIMPL_INT(98)
UNIMPL_INT(99)
UNIMPL_INT(100)
UNIMPL_INT(101)
UNIMPL_INT(102)
UNIMPL_INT(103)
UNIMPL_INT(104)
UNIMPL_INT(105)
UNIMPL_INT(106)
UNIMPL_INT(107)
UNIMPL_INT(108)
UNIMPL_INT(109)
UNIMPL_INT(110)
UNIMPL_INT(111)
UNIMPL_INT(112)
UNIMPL_INT(113)
UNIMPL_INT(114)
UNIMPL_INT(115)
UNIMPL_INT(116)
UNIMPL_INT(117)
UNIMPL_INT(118)
UNIMPL_INT(119)
UNIMPL_INT(120)
UNIMPL_INT(121)
UNIMPL_INT(122)
UNIMPL_INT(123)
UNIMPL_INT(124)
UNIMPL_INT(125)
UNIMPL_INT(126)
UNIMPL_INT(127)
UNIMPL_INT(128)
UNIMPL_INT(129)
UNIMPL_INT(130)
UNIMPL_INT(131)
UNIMPL_INT(132)
UNIMPL_INT(133)
UNIMPL_INT(134)
UNIMPL_INT(135)
UNIMPL_INT(136)
UNIMPL_INT(137)
UNIMPL_INT(138)
UNIMPL_INT(139)
UNIMPL_INT(140)
UNIMPL_INT(141)
UNIMPL_INT(142)
UNIMPL_INT(143)
UNIMPL_INT(144)
UNIMPL_INT(145)
UNIMPL_INT(146)
UNIMPL_INT(147)
UNIMPL_INT(148)
UNIMPL_INT(149)
UNIMPL_INT(150)
UNIMPL_INT(151)
UNIMPL_INT(152)
UNIMPL_INT(153)
UNIMPL_INT(154)
UNIMPL_INT(155)
UNIMPL_INT(156)
UNIMPL_INT(157)
UNIMPL_INT(158)
UNIMPL_INT(159)
UNIMPL_INT(160)
UNIMPL_INT(161)
UNIMPL_INT(162)
UNIMPL_INT(163)
UNIMPL_INT(164)
UNIMPL_INT(165)
UNIMPL_INT(166)
UNIMPL_INT(167)
UNIMPL_INT(168)
UNIMPL_INT(169)
UNIMPL_INT(170)
UNIMPL_INT(171)
UNIMPL_INT(172)
UNIMPL_INT(173)
UNIMPL_INT(174)
UNIMPL_INT(175)
UNIMPL_INT(176)
UNIMPL_INT(177)
UNIMPL_INT(178)
UNIMPL_INT(179)
UNIMPL_INT(180)
UNIMPL_INT(181)
UNIMPL_INT(182)
UNIMPL_INT(183)
UNIMPL_INT(184)
UNIMPL_INT(185)
UNIMPL_INT(186)
UNIMPL_INT(187)
UNIMPL_INT(188)
UNIMPL_INT(189)
UNIMPL_INT(190)
UNIMPL_INT(191)
UNIMPL_INT(192)
UNIMPL_INT(193)
UNIMPL_INT(194)
UNIMPL_INT(195)
UNIMPL_INT(196)
UNIMPL_INT(197)
UNIMPL_INT(198)
UNIMPL_INT(199)
UNIMPL_INT(200)
UNIMPL_INT(201)
UNIMPL_INT(202)
UNIMPL_INT(203)
UNIMPL_INT(204)
UNIMPL_INT(205)
UNIMPL_INT(206)
UNIMPL_INT(207)
UNIMPL_INT(208)
UNIMPL_INT(209)
UNIMPL_INT(210)
UNIMPL_INT(211)
UNIMPL_INT(212)
UNIMPL_INT(213)
UNIMPL_INT(214)
UNIMPL_INT(215)
UNIMPL_INT(216)
UNIMPL_INT(217)
UNIMPL_INT(218)
UNIMPL_INT(219)
UNIMPL_INT(220)
UNIMPL_INT(221)
UNIMPL_INT(222)
UNIMPL_INT(223)
UNIMPL_INT(224)
UNIMPL_INT(225)
UNIMPL_INT(226)
UNIMPL_INT(227)
UNIMPL_INT(228)
UNIMPL_INT(229)
UNIMPL_INT(230)
UNIMPL_INT(231)
UNIMPL_INT(232)
UNIMPL_INT(233)
UNIMPL_INT(234)
UNIMPL_INT(235)
UNIMPL_INT(236)
UNIMPL_INT(237)
UNIMPL_INT(238)
UNIMPL_INT(239)
UNIMPL_INT(240)
UNIMPL_INT(241)
UNIMPL_INT(242)
UNIMPL_INT(243)
UNIMPL_INT(244)
UNIMPL_INT(245)
UNIMPL_INT(246)
UNIMPL_INT(247)
UNIMPL_INT(248)
UNIMPL_INT(249)
UNIMPL_INT(250)
UNIMPL_INT(251)
UNIMPL_INT(252)
UNIMPL_INT(253)
UNIMPL_INT(254)
UNIMPL_INT(255)

static
void
set_all_unimpl_int(void)
{
	set_int(0, (uint32)unimpl_int0);
	set_int(1, (uint32)unimpl_int1);
	set_int(2, (uint32)unimpl_int2);
	set_int(3, (uint32)unimpl_int3);
	set_int(4, (uint32)unimpl_int4);
	set_int(5, (uint32)unimpl_int5);
	set_int(6, (uint32)unimpl_int6);
	set_int(7, (uint32)unimpl_int7);
	set_int(8, (uint32)unimpl_int8);
	set_int(9, (uint32)unimpl_int9);
	set_int(10, (uint32)unimpl_int10);
	set_int(11, (uint32)unimpl_int11);
	set_int(12, (uint32)unimpl_int12);
	set_int(13, (uint32)unimpl_int13);
	set_int(14, (uint32)unimpl_int14);
	set_int(15, (uint32)unimpl_int15);
	set_int(16, (uint32)unimpl_int16);
	set_int(17, (uint32)unimpl_int17);
	set_int(18, (uint32)unimpl_int18);
	set_int(19, (uint32)unimpl_int19);
	set_int(20, (uint32)unimpl_int20);
	set_int(21, (uint32)unimpl_int21);
	set_int(22, (uint32)unimpl_int22);
	set_int(23, (uint32)unimpl_int23);
	set_int(24, (uint32)unimpl_int24);
	set_int(25, (uint32)unimpl_int25);
	set_int(26, (uint32)unimpl_int26);
	set_int(27, (uint32)unimpl_int27);
	set_int(28, (uint32)unimpl_int28);
	set_int(29, (uint32)unimpl_int29);
	set_int(30, (uint32)unimpl_int30);
	set_int(31, (uint32)unimpl_int31);
	set_int(32, (uint32)unimpl_int32);
	set_int(33, (uint32)unimpl_int33);
	set_int(34, (uint32)unimpl_int34);
	set_int(35, (uint32)unimpl_int35);
	set_int(36, (uint32)unimpl_int36);
	set_int(37, (uint32)unimpl_int37);
	set_int(38, (uint32)unimpl_int38);
	set_int(39, (uint32)unimpl_int39);
	set_int(40, (uint32)unimpl_int40);
	set_int(41, (uint32)unimpl_int41);
	set_int(42, (uint32)unimpl_int42);
	set_int(43, (uint32)unimpl_int43);
	set_int(44, (uint32)unimpl_int44);
	set_int(45, (uint32)unimpl_int45);
	set_int(46, (uint32)unimpl_int46);
	set_int(47, (uint32)unimpl_int47);
	set_int(48, (uint32)unimpl_int48);
	set_int(49, (uint32)unimpl_int49);
	set_int(50, (uint32)unimpl_int50);
	set_int(51, (uint32)unimpl_int51);
	set_int(52, (uint32)unimpl_int52);
	set_int(53, (uint32)unimpl_int53);
	set_int(54, (uint32)unimpl_int54);
	set_int(55, (uint32)unimpl_int55);
	set_int(56, (uint32)unimpl_int56);
	set_int(57, (uint32)unimpl_int57);
	set_int(58, (uint32)unimpl_int58);
	set_int(59, (uint32)unimpl_int59);
	set_int(60, (uint32)unimpl_int60);
	set_int(61, (uint32)unimpl_int61);
	set_int(62, (uint32)unimpl_int62);
	set_int(63, (uint32)unimpl_int63);
	set_int(64, (uint32)unimpl_int64);
	set_int(65, (uint32)unimpl_int65);
	set_int(66, (uint32)unimpl_int66);
	set_int(67, (uint32)unimpl_int67);
	set_int(68, (uint32)unimpl_int68);
	set_int(69, (uint32)unimpl_int69);
	set_int(70, (uint32)unimpl_int70);
	set_int(71, (uint32)unimpl_int71);
	set_int(72, (uint32)unimpl_int72);
	set_int(73, (uint32)unimpl_int73);
	set_int(74, (uint32)unimpl_int74);
	set_int(75, (uint32)unimpl_int75);
	set_int(76, (uint32)unimpl_int76);
	set_int(77, (uint32)unimpl_int77);
	set_int(78, (uint32)unimpl_int78);
	set_int(79, (uint32)unimpl_int79);
	set_int(80, (uint32)unimpl_int80);
	set_int(81, (uint32)unimpl_int81);
	set_int(82, (uint32)unimpl_int82);
	set_int(83, (uint32)unimpl_int83);
	set_int(84, (uint32)unimpl_int84);
	set_int(85, (uint32)unimpl_int85);
	set_int(86, (uint32)unimpl_int86);
	set_int(87, (uint32)unimpl_int87);
	set_int(88, (uint32)unimpl_int88);
	set_int(89, (uint32)unimpl_int89);
	set_int(90, (uint32)unimpl_int90);
	set_int(91, (uint32)unimpl_int91);
	set_int(92, (uint32)unimpl_int92);
	set_int(93, (uint32)unimpl_int93);
	set_int(94, (uint32)unimpl_int94);
	set_int(95, (uint32)unimpl_int95);
	set_int(96, (uint32)unimpl_int96);
	set_int(97, (uint32)unimpl_int97);
	set_int(98, (uint32)unimpl_int98);
	set_int(99, (uint32)unimpl_int99);
	set_int(100, (uint32)unimpl_int100);
	set_int(101, (uint32)unimpl_int101);
	set_int(102, (uint32)unimpl_int102);
	set_int(103, (uint32)unimpl_int103);
	set_int(104, (uint32)unimpl_int104);
	set_int(105, (uint32)unimpl_int105);
	set_int(106, (uint32)unimpl_int106);
	set_int(107, (uint32)unimpl_int107);
	set_int(108, (uint32)unimpl_int108);
	set_int(109, (uint32)unimpl_int109);
	set_int(110, (uint32)unimpl_int110);
	set_int(111, (uint32)unimpl_int111);
	set_int(112, (uint32)unimpl_int112);
	set_int(113, (uint32)unimpl_int113);
	set_int(114, (uint32)unimpl_int114);
	set_int(115, (uint32)unimpl_int115);
	set_int(116, (uint32)unimpl_int116);
	set_int(117, (uint32)unimpl_int117);
	set_int(118, (uint32)unimpl_int118);
	set_int(119, (uint32)unimpl_int119);
	set_int(120, (uint32)unimpl_int120);
	set_int(121, (uint32)unimpl_int121);
	set_int(122, (uint32)unimpl_int122);
	set_int(123, (uint32)unimpl_int123);
	set_int(124, (uint32)unimpl_int124);
	set_int(125, (uint32)unimpl_int125);
	set_int(126, (uint32)unimpl_int126);
	set_int(127, (uint32)unimpl_int127);
	set_int(128, (uint32)unimpl_int128);
	set_int(129, (uint32)unimpl_int129);
	set_int(130, (uint32)unimpl_int130);
	set_int(131, (uint32)unimpl_int131);
	set_int(132, (uint32)unimpl_int132);
	set_int(133, (uint32)unimpl_int133);
	set_int(134, (uint32)unimpl_int134);
	set_int(135, (uint32)unimpl_int135);
	set_int(136, (uint32)unimpl_int136);
	set_int(137, (uint32)unimpl_int137);
	set_int(138, (uint32)unimpl_int138);
	set_int(139, (uint32)unimpl_int139);
	set_int(140, (uint32)unimpl_int140);
	set_int(141, (uint32)unimpl_int141);
	set_int(142, (uint32)unimpl_int142);
	set_int(143, (uint32)unimpl_int143);
	set_int(144, (uint32)unimpl_int144);
	set_int(145, (uint32)unimpl_int145);
	set_int(146, (uint32)unimpl_int146);
	set_int(147, (uint32)unimpl_int147);
	set_int(148, (uint32)unimpl_int148);
	set_int(149, (uint32)unimpl_int149);
	set_int(150, (uint32)unimpl_int150);
	set_int(151, (uint32)unimpl_int151);
	set_int(152, (uint32)unimpl_int152);
	set_int(153, (uint32)unimpl_int153);
	set_int(154, (uint32)unimpl_int154);
	set_int(155, (uint32)unimpl_int155);
	set_int(156, (uint32)unimpl_int156);
	set_int(157, (uint32)unimpl_int157);
	set_int(158, (uint32)unimpl_int158);
	set_int(159, (uint32)unimpl_int159);
	set_int(160, (uint32)unimpl_int160);
	set_int(161, (uint32)unimpl_int161);
	set_int(162, (uint32)unimpl_int162);
	set_int(163, (uint32)unimpl_int163);
	set_int(164, (uint32)unimpl_int164);
	set_int(165, (uint32)unimpl_int165);
	set_int(166, (uint32)unimpl_int166);
	set_int(167, (uint32)unimpl_int167);
	set_int(168, (uint32)unimpl_int168);
	set_int(169, (uint32)unimpl_int169);
	set_int(170, (uint32)unimpl_int170);
	set_int(171, (uint32)unimpl_int171);
	set_int(172, (uint32)unimpl_int172);
	set_int(173, (uint32)unimpl_int173);
	set_int(174, (uint32)unimpl_int174);
	set_int(175, (uint32)unimpl_int175);
	set_int(176, (uint32)unimpl_int176);
	set_int(177, (uint32)unimpl_int177);
	set_int(178, (uint32)unimpl_int178);
	set_int(179, (uint32)unimpl_int179);
	set_int(180, (uint32)unimpl_int180);
	set_int(181, (uint32)unimpl_int181);
	set_int(182, (uint32)unimpl_int182);
	set_int(183, (uint32)unimpl_int183);
	set_int(184, (uint32)unimpl_int184);
	set_int(185, (uint32)unimpl_int185);
	set_int(186, (uint32)unimpl_int186);
	set_int(187, (uint32)unimpl_int187);
	set_int(188, (uint32)unimpl_int188);
	set_int(189, (uint32)unimpl_int189);
	set_int(190, (uint32)unimpl_int190);
	set_int(191, (uint32)unimpl_int191);
	set_int(192, (uint32)unimpl_int192);
	set_int(193, (uint32)unimpl_int193);
	set_int(194, (uint32)unimpl_int194);
	set_int(195, (uint32)unimpl_int195);
	set_int(196, (uint32)unimpl_int196);
	set_int(197, (uint32)unimpl_int197);
	set_int(198, (uint32)unimpl_int198);
	set_int(199, (uint32)unimpl_int199);
	set_int(200, (uint32)unimpl_int200);
	set_int(201, (uint32)unimpl_int201);
	set_int(202, (uint32)unimpl_int202);
	set_int(203, (uint32)unimpl_int203);
	set_int(204, (uint32)unimpl_int204);
	set_int(205, (uint32)unimpl_int205);
	set_int(206, (uint32)unimpl_int206);
	set_int(207, (uint32)unimpl_int207);
	set_int(208, (uint32)unimpl_int208);
	set_int(209, (uint32)unimpl_int209);
	set_int(210, (uint32)unimpl_int210);
	set_int(211, (uint32)unimpl_int211);
	set_int(212, (uint32)unimpl_int212);
	set_int(213, (uint32)unimpl_int213);
	set_int(214, (uint32)unimpl_int214);
	set_int(215, (uint32)unimpl_int215);
	set_int(216, (uint32)unimpl_int216);
	set_int(217, (uint32)unimpl_int217);
	set_int(218, (uint32)unimpl_int218);
	set_int(219, (uint32)unimpl_int219);
	set_int(220, (uint32)unimpl_int220);
	set_int(221, (uint32)unimpl_int221);
	set_int(222, (uint32)unimpl_int222);
	set_int(223, (uint32)unimpl_int223);
	set_int(224, (uint32)unimpl_int224);
	set_int(225, (uint32)unimpl_int225);
	set_int(226, (uint32)unimpl_int226);
	set_int(227, (uint32)unimpl_int227);
	set_int(228, (uint32)unimpl_int228);
	set_int(229, (uint32)unimpl_int229);
	set_int(230, (uint32)unimpl_int230);
	set_int(231, (uint32)unimpl_int231);
	set_int(232, (uint32)unimpl_int232);
	set_int(233, (uint32)unimpl_int233);
	set_int(234, (uint32)unimpl_int234);
	set_int(235, (uint32)unimpl_int235);
	set_int(236, (uint32)unimpl_int236);
	set_int(237, (uint32)unimpl_int237);
	set_int(238, (uint32)unimpl_int238);
	set_int(239, (uint32)unimpl_int239);
	set_int(240, (uint32)unimpl_int240);
	set_int(241, (uint32)unimpl_int241);
	set_int(242, (uint32)unimpl_int242);
	set_int(243, (uint32)unimpl_int243);
	set_int(244, (uint32)unimpl_int244);
	set_int(245, (uint32)unimpl_int245);
	set_int(246, (uint32)unimpl_int246);
	set_int(247, (uint32)unimpl_int247);
	set_int(248, (uint32)unimpl_int248);
	set_int(249, (uint32)unimpl_int249);
	set_int(250, (uint32)unimpl_int250);
	set_int(251, (uint32)unimpl_int251);
	set_int(252, (uint32)unimpl_int252);
	set_int(253, (uint32)unimpl_int253);
	set_int(254, (uint32)unimpl_int254);
	set_int(255, (uint32)unimpl_int255);
}

BOOL
interrupts_init(void)
{
	set_all_unimpl_int();
	return TRUE;
}

int32
get_unimpl_intn(void)
{
	return unimpl_intn;
}
