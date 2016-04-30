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
	KnlSetInterrupt(0, (uint32)unimpl_int0);
	KnlSetInterrupt(1, (uint32)unimpl_int1);
	KnlSetInterrupt(2, (uint32)unimpl_int2);
	KnlSetInterrupt(3, (uint32)unimpl_int3);
	KnlSetInterrupt(4, (uint32)unimpl_int4);
	KnlSetInterrupt(5, (uint32)unimpl_int5);
	KnlSetInterrupt(6, (uint32)unimpl_int6);
	KnlSetInterrupt(7, (uint32)unimpl_int7);
	KnlSetInterrupt(8, (uint32)unimpl_int8);
	KnlSetInterrupt(9, (uint32)unimpl_int9);
	KnlSetInterrupt(10, (uint32)unimpl_int10);
	KnlSetInterrupt(11, (uint32)unimpl_int11);
	KnlSetInterrupt(12, (uint32)unimpl_int12);
	KnlSetInterrupt(13, (uint32)unimpl_int13);
	KnlSetInterrupt(14, (uint32)unimpl_int14);
	KnlSetInterrupt(15, (uint32)unimpl_int15);
	KnlSetInterrupt(16, (uint32)unimpl_int16);
	KnlSetInterrupt(17, (uint32)unimpl_int17);
	KnlSetInterrupt(18, (uint32)unimpl_int18);
	KnlSetInterrupt(19, (uint32)unimpl_int19);
	KnlSetInterrupt(20, (uint32)unimpl_int20);
	KnlSetInterrupt(21, (uint32)unimpl_int21);
	KnlSetInterrupt(22, (uint32)unimpl_int22);
	KnlSetInterrupt(23, (uint32)unimpl_int23);
	KnlSetInterrupt(24, (uint32)unimpl_int24);
	KnlSetInterrupt(25, (uint32)unimpl_int25);
	KnlSetInterrupt(26, (uint32)unimpl_int26);
	KnlSetInterrupt(27, (uint32)unimpl_int27);
	KnlSetInterrupt(28, (uint32)unimpl_int28);
	KnlSetInterrupt(29, (uint32)unimpl_int29);
	KnlSetInterrupt(30, (uint32)unimpl_int30);
	KnlSetInterrupt(31, (uint32)unimpl_int31);
	KnlSetInterrupt(32, (uint32)unimpl_int32);
	KnlSetInterrupt(33, (uint32)unimpl_int33);
	KnlSetInterrupt(34, (uint32)unimpl_int34);
	KnlSetInterrupt(35, (uint32)unimpl_int35);
	KnlSetInterrupt(36, (uint32)unimpl_int36);
	KnlSetInterrupt(37, (uint32)unimpl_int37);
	KnlSetInterrupt(38, (uint32)unimpl_int38);
	KnlSetInterrupt(39, (uint32)unimpl_int39);
	KnlSetInterrupt(40, (uint32)unimpl_int40);
	KnlSetInterrupt(41, (uint32)unimpl_int41);
	KnlSetInterrupt(42, (uint32)unimpl_int42);
	KnlSetInterrupt(43, (uint32)unimpl_int43);
	KnlSetInterrupt(44, (uint32)unimpl_int44);
	KnlSetInterrupt(45, (uint32)unimpl_int45);
	KnlSetInterrupt(46, (uint32)unimpl_int46);
	KnlSetInterrupt(47, (uint32)unimpl_int47);
	KnlSetInterrupt(48, (uint32)unimpl_int48);
	KnlSetInterrupt(49, (uint32)unimpl_int49);
	KnlSetInterrupt(50, (uint32)unimpl_int50);
	KnlSetInterrupt(51, (uint32)unimpl_int51);
	KnlSetInterrupt(52, (uint32)unimpl_int52);
	KnlSetInterrupt(53, (uint32)unimpl_int53);
	KnlSetInterrupt(54, (uint32)unimpl_int54);
	KnlSetInterrupt(55, (uint32)unimpl_int55);
	KnlSetInterrupt(56, (uint32)unimpl_int56);
	KnlSetInterrupt(57, (uint32)unimpl_int57);
	KnlSetInterrupt(58, (uint32)unimpl_int58);
	KnlSetInterrupt(59, (uint32)unimpl_int59);
	KnlSetInterrupt(60, (uint32)unimpl_int60);
	KnlSetInterrupt(61, (uint32)unimpl_int61);
	KnlSetInterrupt(62, (uint32)unimpl_int62);
	KnlSetInterrupt(63, (uint32)unimpl_int63);
	KnlSetInterrupt(64, (uint32)unimpl_int64);
	KnlSetInterrupt(65, (uint32)unimpl_int65);
	KnlSetInterrupt(66, (uint32)unimpl_int66);
	KnlSetInterrupt(67, (uint32)unimpl_int67);
	KnlSetInterrupt(68, (uint32)unimpl_int68);
	KnlSetInterrupt(69, (uint32)unimpl_int69);
	KnlSetInterrupt(70, (uint32)unimpl_int70);
	KnlSetInterrupt(71, (uint32)unimpl_int71);
	KnlSetInterrupt(72, (uint32)unimpl_int72);
	KnlSetInterrupt(73, (uint32)unimpl_int73);
	KnlSetInterrupt(74, (uint32)unimpl_int74);
	KnlSetInterrupt(75, (uint32)unimpl_int75);
	KnlSetInterrupt(76, (uint32)unimpl_int76);
	KnlSetInterrupt(77, (uint32)unimpl_int77);
	KnlSetInterrupt(78, (uint32)unimpl_int78);
	KnlSetInterrupt(79, (uint32)unimpl_int79);
	KnlSetInterrupt(80, (uint32)unimpl_int80);
	KnlSetInterrupt(81, (uint32)unimpl_int81);
	KnlSetInterrupt(82, (uint32)unimpl_int82);
	KnlSetInterrupt(83, (uint32)unimpl_int83);
	KnlSetInterrupt(84, (uint32)unimpl_int84);
	KnlSetInterrupt(85, (uint32)unimpl_int85);
	KnlSetInterrupt(86, (uint32)unimpl_int86);
	KnlSetInterrupt(87, (uint32)unimpl_int87);
	KnlSetInterrupt(88, (uint32)unimpl_int88);
	KnlSetInterrupt(89, (uint32)unimpl_int89);
	KnlSetInterrupt(90, (uint32)unimpl_int90);
	KnlSetInterrupt(91, (uint32)unimpl_int91);
	KnlSetInterrupt(92, (uint32)unimpl_int92);
	KnlSetInterrupt(93, (uint32)unimpl_int93);
	KnlSetInterrupt(94, (uint32)unimpl_int94);
	KnlSetInterrupt(95, (uint32)unimpl_int95);
	KnlSetInterrupt(96, (uint32)unimpl_int96);
	KnlSetInterrupt(97, (uint32)unimpl_int97);
	KnlSetInterrupt(98, (uint32)unimpl_int98);
	KnlSetInterrupt(99, (uint32)unimpl_int99);
	KnlSetInterrupt(100, (uint32)unimpl_int100);
	KnlSetInterrupt(101, (uint32)unimpl_int101);
	KnlSetInterrupt(102, (uint32)unimpl_int102);
	KnlSetInterrupt(103, (uint32)unimpl_int103);
	KnlSetInterrupt(104, (uint32)unimpl_int104);
	KnlSetInterrupt(105, (uint32)unimpl_int105);
	KnlSetInterrupt(106, (uint32)unimpl_int106);
	KnlSetInterrupt(107, (uint32)unimpl_int107);
	KnlSetInterrupt(108, (uint32)unimpl_int108);
	KnlSetInterrupt(109, (uint32)unimpl_int109);
	KnlSetInterrupt(110, (uint32)unimpl_int110);
	KnlSetInterrupt(111, (uint32)unimpl_int111);
	KnlSetInterrupt(112, (uint32)unimpl_int112);
	KnlSetInterrupt(113, (uint32)unimpl_int113);
	KnlSetInterrupt(114, (uint32)unimpl_int114);
	KnlSetInterrupt(115, (uint32)unimpl_int115);
	KnlSetInterrupt(116, (uint32)unimpl_int116);
	KnlSetInterrupt(117, (uint32)unimpl_int117);
	KnlSetInterrupt(118, (uint32)unimpl_int118);
	KnlSetInterrupt(119, (uint32)unimpl_int119);
	KnlSetInterrupt(120, (uint32)unimpl_int120);
	KnlSetInterrupt(121, (uint32)unimpl_int121);
	KnlSetInterrupt(122, (uint32)unimpl_int122);
	KnlSetInterrupt(123, (uint32)unimpl_int123);
	KnlSetInterrupt(124, (uint32)unimpl_int124);
	KnlSetInterrupt(125, (uint32)unimpl_int125);
	KnlSetInterrupt(126, (uint32)unimpl_int126);
	KnlSetInterrupt(127, (uint32)unimpl_int127);
	KnlSetInterrupt(128, (uint32)unimpl_int128);
	KnlSetInterrupt(129, (uint32)unimpl_int129);
	KnlSetInterrupt(130, (uint32)unimpl_int130);
	KnlSetInterrupt(131, (uint32)unimpl_int131);
	KnlSetInterrupt(132, (uint32)unimpl_int132);
	KnlSetInterrupt(133, (uint32)unimpl_int133);
	KnlSetInterrupt(134, (uint32)unimpl_int134);
	KnlSetInterrupt(135, (uint32)unimpl_int135);
	KnlSetInterrupt(136, (uint32)unimpl_int136);
	KnlSetInterrupt(137, (uint32)unimpl_int137);
	KnlSetInterrupt(138, (uint32)unimpl_int138);
	KnlSetInterrupt(139, (uint32)unimpl_int139);
	KnlSetInterrupt(140, (uint32)unimpl_int140);
	KnlSetInterrupt(141, (uint32)unimpl_int141);
	KnlSetInterrupt(142, (uint32)unimpl_int142);
	KnlSetInterrupt(143, (uint32)unimpl_int143);
	KnlSetInterrupt(144, (uint32)unimpl_int144);
	KnlSetInterrupt(145, (uint32)unimpl_int145);
	KnlSetInterrupt(146, (uint32)unimpl_int146);
	KnlSetInterrupt(147, (uint32)unimpl_int147);
	KnlSetInterrupt(148, (uint32)unimpl_int148);
	KnlSetInterrupt(149, (uint32)unimpl_int149);
	KnlSetInterrupt(150, (uint32)unimpl_int150);
	KnlSetInterrupt(151, (uint32)unimpl_int151);
	KnlSetInterrupt(152, (uint32)unimpl_int152);
	KnlSetInterrupt(153, (uint32)unimpl_int153);
	KnlSetInterrupt(154, (uint32)unimpl_int154);
	KnlSetInterrupt(155, (uint32)unimpl_int155);
	KnlSetInterrupt(156, (uint32)unimpl_int156);
	KnlSetInterrupt(157, (uint32)unimpl_int157);
	KnlSetInterrupt(158, (uint32)unimpl_int158);
	KnlSetInterrupt(159, (uint32)unimpl_int159);
	KnlSetInterrupt(160, (uint32)unimpl_int160);
	KnlSetInterrupt(161, (uint32)unimpl_int161);
	KnlSetInterrupt(162, (uint32)unimpl_int162);
	KnlSetInterrupt(163, (uint32)unimpl_int163);
	KnlSetInterrupt(164, (uint32)unimpl_int164);
	KnlSetInterrupt(165, (uint32)unimpl_int165);
	KnlSetInterrupt(166, (uint32)unimpl_int166);
	KnlSetInterrupt(167, (uint32)unimpl_int167);
	KnlSetInterrupt(168, (uint32)unimpl_int168);
	KnlSetInterrupt(169, (uint32)unimpl_int169);
	KnlSetInterrupt(170, (uint32)unimpl_int170);
	KnlSetInterrupt(171, (uint32)unimpl_int171);
	KnlSetInterrupt(172, (uint32)unimpl_int172);
	KnlSetInterrupt(173, (uint32)unimpl_int173);
	KnlSetInterrupt(174, (uint32)unimpl_int174);
	KnlSetInterrupt(175, (uint32)unimpl_int175);
	KnlSetInterrupt(176, (uint32)unimpl_int176);
	KnlSetInterrupt(177, (uint32)unimpl_int177);
	KnlSetInterrupt(178, (uint32)unimpl_int178);
	KnlSetInterrupt(179, (uint32)unimpl_int179);
	KnlSetInterrupt(180, (uint32)unimpl_int180);
	KnlSetInterrupt(181, (uint32)unimpl_int181);
	KnlSetInterrupt(182, (uint32)unimpl_int182);
	KnlSetInterrupt(183, (uint32)unimpl_int183);
	KnlSetInterrupt(184, (uint32)unimpl_int184);
	KnlSetInterrupt(185, (uint32)unimpl_int185);
	KnlSetInterrupt(186, (uint32)unimpl_int186);
	KnlSetInterrupt(187, (uint32)unimpl_int187);
	KnlSetInterrupt(188, (uint32)unimpl_int188);
	KnlSetInterrupt(189, (uint32)unimpl_int189);
	KnlSetInterrupt(190, (uint32)unimpl_int190);
	KnlSetInterrupt(191, (uint32)unimpl_int191);
	KnlSetInterrupt(192, (uint32)unimpl_int192);
	KnlSetInterrupt(193, (uint32)unimpl_int193);
	KnlSetInterrupt(194, (uint32)unimpl_int194);
	KnlSetInterrupt(195, (uint32)unimpl_int195);
	KnlSetInterrupt(196, (uint32)unimpl_int196);
	KnlSetInterrupt(197, (uint32)unimpl_int197);
	KnlSetInterrupt(198, (uint32)unimpl_int198);
	KnlSetInterrupt(199, (uint32)unimpl_int199);
	KnlSetInterrupt(200, (uint32)unimpl_int200);
	KnlSetInterrupt(201, (uint32)unimpl_int201);
	KnlSetInterrupt(202, (uint32)unimpl_int202);
	KnlSetInterrupt(203, (uint32)unimpl_int203);
	KnlSetInterrupt(204, (uint32)unimpl_int204);
	KnlSetInterrupt(205, (uint32)unimpl_int205);
	KnlSetInterrupt(206, (uint32)unimpl_int206);
	KnlSetInterrupt(207, (uint32)unimpl_int207);
	KnlSetInterrupt(208, (uint32)unimpl_int208);
	KnlSetInterrupt(209, (uint32)unimpl_int209);
	KnlSetInterrupt(210, (uint32)unimpl_int210);
	KnlSetInterrupt(211, (uint32)unimpl_int211);
	KnlSetInterrupt(212, (uint32)unimpl_int212);
	KnlSetInterrupt(213, (uint32)unimpl_int213);
	KnlSetInterrupt(214, (uint32)unimpl_int214);
	KnlSetInterrupt(215, (uint32)unimpl_int215);
	KnlSetInterrupt(216, (uint32)unimpl_int216);
	KnlSetInterrupt(217, (uint32)unimpl_int217);
	KnlSetInterrupt(218, (uint32)unimpl_int218);
	KnlSetInterrupt(219, (uint32)unimpl_int219);
	KnlSetInterrupt(220, (uint32)unimpl_int220);
	KnlSetInterrupt(221, (uint32)unimpl_int221);
	KnlSetInterrupt(222, (uint32)unimpl_int222);
	KnlSetInterrupt(223, (uint32)unimpl_int223);
	KnlSetInterrupt(224, (uint32)unimpl_int224);
	KnlSetInterrupt(225, (uint32)unimpl_int225);
	KnlSetInterrupt(226, (uint32)unimpl_int226);
	KnlSetInterrupt(227, (uint32)unimpl_int227);
	KnlSetInterrupt(228, (uint32)unimpl_int228);
	KnlSetInterrupt(229, (uint32)unimpl_int229);
	KnlSetInterrupt(230, (uint32)unimpl_int230);
	KnlSetInterrupt(231, (uint32)unimpl_int231);
	KnlSetInterrupt(232, (uint32)unimpl_int232);
	KnlSetInterrupt(233, (uint32)unimpl_int233);
	KnlSetInterrupt(234, (uint32)unimpl_int234);
	KnlSetInterrupt(235, (uint32)unimpl_int235);
	KnlSetInterrupt(236, (uint32)unimpl_int236);
	KnlSetInterrupt(237, (uint32)unimpl_int237);
	KnlSetInterrupt(238, (uint32)unimpl_int238);
	KnlSetInterrupt(239, (uint32)unimpl_int239);
	KnlSetInterrupt(240, (uint32)unimpl_int240);
	KnlSetInterrupt(241, (uint32)unimpl_int241);
	KnlSetInterrupt(242, (uint32)unimpl_int242);
	KnlSetInterrupt(243, (uint32)unimpl_int243);
	KnlSetInterrupt(244, (uint32)unimpl_int244);
	KnlSetInterrupt(245, (uint32)unimpl_int245);
	KnlSetInterrupt(246, (uint32)unimpl_int246);
	KnlSetInterrupt(247, (uint32)unimpl_int247);
	KnlSetInterrupt(248, (uint32)unimpl_int248);
	KnlSetInterrupt(249, (uint32)unimpl_int249);
	KnlSetInterrupt(250, (uint32)unimpl_int250);
	KnlSetInterrupt(251, (uint32)unimpl_int251);
	KnlSetInterrupt(252, (uint32)unimpl_int252);
	KnlSetInterrupt(253, (uint32)unimpl_int253);
	KnlSetInterrupt(254, (uint32)unimpl_int254);
	KnlSetInterrupt(255, (uint32)unimpl_int255);
}

BOOL
KnlInitInterrupts(void)
{
	set_all_unimpl_int();
	return TRUE;
}

int32
KnlGetUnimplementedInterruptNo(void)
{
	return unimpl_intn;
}
