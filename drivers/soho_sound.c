#include <config.h>
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/mcbsp.h>
#include "soho_sound.h"

#define MAX98090_I2C_BUS	2
#define MAX98090_I2C_ADDR	0x10

#define MAX98090_MAX_REG	0x46

extern int select_bus(int bus, int speed);

static short click[] = {
0 ,-1 ,2 ,-1 ,0 ,0 ,0 ,-1 ,
-2 ,2 ,-2 ,2 ,-2 ,2 ,-1 ,0 ,
1 ,-1 ,1 ,-1 ,1 ,-1 ,1 ,-1 ,
1 ,-1 ,1 ,-1 ,0 ,1 ,-1 ,1 ,
1 ,-1 ,1 ,-1 ,0 ,2 ,-4 ,4 ,
0 ,2 ,-3 ,2 ,-1 ,1 ,-1 ,2 ,
2 ,-1 ,1 ,0 ,-1 ,0 ,1 ,0 ,
0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,
1 ,0 ,-1 ,3 ,-3 ,2 ,-2 ,1 ,
0 ,0 ,1 ,-2 ,2 ,-2 ,2 ,-2 ,
-5 ,6 ,-5 ,3 ,-2 ,1 ,0 ,1 ,
-2 ,3 ,-4 ,4 ,-4 ,4 ,-4 ,5 ,
1 ,0 ,-1 ,0 ,0 ,-1 ,1 ,0 ,
-1 ,2 ,-2 ,2 ,-2 ,0 ,0 ,-1 ,
-1 ,2 ,-1 ,0 ,-1 ,0 ,1 ,-3 ,
-4 ,3 ,0 ,1 ,0 ,0 ,0 ,-1 ,
-2 ,1 ,1 ,-2 ,3 ,-1 ,2 ,0 ,
0 ,-2 ,-1 ,0 ,-2 ,2 ,0 ,2 ,
0 ,-1 ,-2 ,-2 ,-1 ,-1 ,2 ,2 ,
4 ,-2 ,2 ,-4 ,-3 ,-2 ,-2 ,2 ,
3 ,3 ,0 ,3 ,-4 ,0 ,-6 ,-1 ,
1 ,5 ,1 ,3 ,-1 ,-2 ,-1 ,-5 ,
-2 ,0 ,3 ,5 ,2 ,4 ,-1 ,2 ,
3 ,-4 ,1 ,0 ,1 ,8 ,2 ,8 ,
2 ,-8 ,-3 ,-11 ,-2 ,-5 ,4 ,8 ,
8 ,-1 ,-6 ,-8 ,-11 ,-6 ,-7 ,5 ,
16 ,15 ,5 ,-3 ,-18 ,-20 ,-17 ,-8 ,
12 ,18 ,22 ,16 ,4 ,-13 ,-27 ,-27 ,
3 ,19 ,22 ,19 ,5 ,-4 ,-16 ,-22 ,
-22 ,-10 ,9 ,23 ,31 ,23 ,14 ,3 ,
-1 ,2 ,-5 ,-5 ,6 ,16 ,38 ,35 ,
-3 ,-27 ,-40 ,-36 ,-25 ,-1 ,7 ,29 ,
28 ,6 ,-24 ,-54 ,-53 ,-37 ,10 ,37 ,
28 ,-3 ,-23 ,-28 ,-18 ,-10 ,-9 ,-2 ,
20 ,21 ,6 ,-20 ,-36 ,-29 ,2 ,18 ,
-18 ,-48 ,-35 ,-6 ,38 ,42 ,20 ,-18 ,
-20 ,-9 ,-1 ,-23 ,-20 ,-4 ,35 ,53 ,
-29 ,-75 ,-56 ,3 ,60 ,72 ,34 ,-8 ,
13 ,40 ,35 ,-5 ,-33 ,-32 ,10 ,27 ,
-39 ,-60 ,-30 ,36 ,75 ,47 ,-32 ,-93 ,
-14 ,63 ,82 ,41 ,2 ,-22 ,-6 ,-14 ,
-98 ,-95 ,-30 ,71 ,130 ,96 ,2 ,-79 ,
-27 ,43 ,53 ,33 ,7 ,24 ,66 ,75 ,
-60 ,-114 ,-93 ,-17 ,46 ,49 ,12 ,-13 ,
78 ,100 ,53 ,-43 ,-103 ,-78 ,18 ,102 ,
72 ,13 ,-32 ,-31 ,-52 ,-84 ,-128 ,-105 ,
123 ,201 ,160 ,34 ,-86 ,-129 ,-81 ,0 ,
55 ,51 ,72 ,93 ,71 ,-15 ,-137 ,-199 ,
-47 ,67 ,101 ,64 ,6 ,-6 ,5 ,11 ,
-102 ,-120 ,-61 ,62 ,159 ,163 ,90 ,-14 ,
-73 ,-63 ,-73 ,-79 ,-51 ,45 ,147 ,183 ,
-45 ,-177 ,-210 ,-142 ,-31 ,55 ,97 ,104 ,
98 ,50 ,-72 ,-168 ,-213 ,-129 ,9 ,133 ,
107 ,13 ,-65 ,-104 ,-130 ,-143 ,-136 ,-72 ,
159 ,212 ,175 ,58 ,-59 ,-126 ,-133 ,-90 ,
10 ,70 ,119 ,138 ,79 ,-34 ,-163 ,-238 ,
-115 ,15 ,139 ,196 ,205 ,154 ,65 ,-50 ,
-186 ,-140 ,-39 ,90 ,176 ,193 ,134 ,21 ,
-194 ,-218 ,-180 ,-64 ,68 ,192 ,248 ,225 ,
5 ,-115 ,-200 ,-208 ,-147 ,-21 ,114 ,205 ,
118 ,-18 ,-141 ,-201 ,-194 ,-117 ,-7 ,132 ,
306 ,257 ,128 ,-60 ,-198 ,-250 ,-183 ,-66 ,
123 ,143 ,134 ,87 ,-3 ,-122 ,-219 ,-237 ,
44 ,209 ,289 ,253 ,155 ,47 ,-43 ,-122 ,
-216 ,-150 ,-3 ,178 ,276 ,232 ,55 ,-134 ,
-239 ,-157 ,-55 ,31 ,118 ,208 ,260 ,203 ,
-194 ,-325 ,-296 ,-112 ,84 ,193 ,166 ,91 ,
0 ,-44 ,-125 ,-219 ,-223 ,-100 ,102 ,251 ,
119 ,-26 ,-91 ,-62 ,-28 ,-48 ,-108 ,-103 ,
188 ,287 ,201 ,-22 ,-231 ,-310 ,-226 ,-101 ,
-10 ,15 ,90 ,178 ,162 ,2 ,-221 ,-332 ,
29 ,264 ,320 ,199 ,20 ,-77 ,-79 ,-73 ,
-201 ,-198 ,-67 ,128 ,234 ,152 ,-47 ,-201 ,
-25 ,119 ,123 ,21 ,-63 ,-15 ,123 ,206 ,
-54 ,-205 ,-214 ,-88 ,29 ,45 ,-24 ,-55 ,
171 ,224 ,101 ,-114 ,-243 ,-155 ,77 ,285 ,
141 ,-43 ,-127 ,-82 ,-37 ,-45 ,-122 ,-111 ,
183 ,248 ,134 ,-91 ,-229 ,-171 ,39 ,216 ,
49 ,-126 ,-158 ,-67 ,29 ,29 ,-68 ,-122 ,
46 ,105 ,39 ,-99 ,-151 ,-35 ,177 ,311 ,
38 ,-145 ,-158 ,-23 ,119 ,146 ,46 ,-50 ,
12 ,63 ,11 ,-88 ,-127 ,-28 ,123 ,206 ,
-59 ,-168 ,-113 ,71 ,202 ,184 ,8 ,-164 ,
-145 ,-32 ,29 ,29 ,34 ,75 ,113 ,63 ,
-256 ,-287 ,-127 ,137 ,332 ,320 ,135 ,-99 ,
-230 ,-148 ,-71 ,-5 ,50 ,128 ,162 ,104 ,
-234 ,-318 ,-247 ,-87 ,52 ,88 ,29 ,-26 ,
30 ,67 ,51 ,-28 ,-79 ,-76 ,5 ,105 ,
220 ,218 ,182 ,83 ,-67 ,-234 ,-332 ,-282 ,
159 ,335 ,356 ,200 ,-50 ,-281 ,-408 ,-367 ,
75 ,310 ,426 ,359 ,141 ,-150 ,-375 ,-466 ,
-188 ,47 ,234 ,310 ,262 ,102 ,-82 ,-222 ,
-169 ,-36 ,80 ,145 ,188 ,210 ,220 ,175 ,
-143 ,-307 ,-343 ,-252 ,-55 ,136 ,274 ,342 ,
204 ,-22 ,-298 ,-502 ,-486 ,-212 ,226 ,582 ,
464 ,81 ,-275 ,-466 ,-466 ,-320 ,-84 ,225 ,
705 ,615 ,248 ,-238 ,-575 ,-597 ,-307 ,63 ,
332 ,236 ,149 ,76 ,-38 ,-247 ,-480 ,-550 ,
-13 ,318 ,433 ,327 ,149 ,45 ,9 ,-102 ,
-540 ,-562 ,-212 ,318 ,726 ,733 ,333 ,-187 ,
-599 ,-439 ,-255 ,-43 ,255 ,623 ,890 ,786 ,
-515 ,-1015 ,-915 ,-373 ,313 ,727 ,661 ,392 ,
-44 ,-217 ,-477 ,-682 ,-658 ,-224 ,428 ,821 ,
271 ,-249 ,-432 ,-377 ,-191 ,-200 ,-376 ,-307 ,
579 ,925 ,729 ,111 ,-570 ,-802 ,-622 ,-369 ,
16 ,137 ,507 ,778 ,642 ,-19 ,-831 ,-1303 ,
-286 ,490 ,797 ,735 ,322 ,93 ,15 ,-181 ,
-820 ,-728 ,-13 ,702 ,1099 ,848 ,190 ,-445 ,
-266 ,-312 ,-342 ,-185 ,-256 ,407 ,1116 ,1088 ,
-390 ,-1096 ,-1106 ,-621 ,218 ,439 ,466 ,573 ,
763 ,352 ,-428 ,-1184 ,-1711 ,-889 ,173 ,866 ,
854 ,217 ,-190 ,-348 ,-509 ,-1029 ,-702 ,-552 ,
762 ,936 ,695 ,464 ,-339 ,-486 ,-474 ,259 ,
25 ,299 ,-168 ,-697 ,-64 ,-628 ,-589 ,-265 ,
1130 ,759 ,575 ,507 ,-906 ,-948 ,-806 ,-67 ,
481 ,1279 ,-87 ,-1417 ,-222 ,-1683 ,-1016 ,1879 ,
695 ,113 ,-1282 ,-1828 ,-1366 ,-342 ,-974 ,159 ,
1635 ,1813 ,1068 ,-224 ,-341 ,-2844 ,-2959 ,-1371 ,
946 ,1994 ,1383 ,1397 ,1395 ,816 ,-1726 ,-1159 ,
-1368 ,1709 ,1512 ,-2382 ,-374 ,91 ,-591 ,1179 ,
302 ,489 ,414 ,-32 ,-1047 ,-225 ,9 ,167 ,
-310 ,-101 ,47 ,-222 ,728 ,-82 ,-765 ,-689 ,
323 ,740 ,771 ,6 ,-391 ,390 ,310 ,1252 ,
-1167 ,-752 ,-878 ,40 ,1222 ,367 ,414 ,-443 ,
838 ,-430 ,-30 ,-798 ,-1499 ,-54 ,18 ,1717 ,
805 ,104 ,-1433 ,1423 ,1992 ,-293 ,310 ,-4557 ,
4715 ,3245 ,908 ,-2091 ,-7521 ,-5497 ,-936 ,2680 ,
4854 ,7668 ,5434 ,3983 ,478 ,-7272 ,-7461 ,-6174 ,
3289 ,5177 ,7892 ,6457 ,-1253 ,-3280 ,-1759 ,-2007 ,
-2667 ,164 ,1389 ,9827 ,8722 ,-3159 ,-4533 ,-7295 ,
2640 ,3447 ,4975 ,5632 ,436 ,-522 ,-4164 ,-5940 ,
-2572 ,1339 ,3504 ,4930 ,5786 ,1073 ,-786 ,-4250 ,
750 ,1063 ,1168 ,2940 ,-919 ,214 ,2140 ,1083 ,
617 ,1024 ,500 ,-2485 ,-2492 ,-3389 ,-3079 ,-192 ,
-1004 ,-971 ,-41 ,335 ,-557 ,378 ,1250 ,1230 ,
279 ,-258 ,-633 ,63 ,1074 ,119 ,935 ,2055 ,
3204 ,2762 ,171 ,-452 ,-1264 ,-1881 ,-1257 ,-1253 ,
-431 ,-149 ,116 ,-133 ,-354 ,-217 ,-437 ,-218 ,
463 ,860 ,-8 ,-283 ,934 ,1001 ,1556 ,1592 ,
-489 ,-430 ,-583 ,915 ,846 ,560 ,497 ,-1230 ,
-315 ,-319 ,-398 ,-527 ,253 ,100 ,-643 ,-628 ,
-1713 ,1211 ,1535 ,1550 ,1743 ,529 ,542 ,1177 ,
394 ,-98 ,-414 ,-469 ,-412 ,25 ,642 ,405 ,
-314 ,-1695 ,-1694 ,-478 ,-153 ,677 ,1088 ,856 ,
-1759 ,-2140 ,-779 ,-134 ,1936 ,2572 ,1397 ,1240 ,
-911 ,-106 ,-386 ,471 ,901 ,36 ,224 ,-412 ,
324 ,-215 ,-637 ,-638 ,-1515 ,-1126 ,-151 ,145 ,
840 ,-71 ,-290 ,-984 ,-836 ,778 ,1192 ,1412 ,
232 ,-323 ,-476 ,-782 ,-171 ,14 ,439 ,753 ,
3 ,198 ,-313 ,-50 ,-479 ,-942 ,-842 ,-1217 ,
209 ,532 ,1205 ,944 ,74 ,-226 ,-340 ,-36 ,
923 ,1458 ,1314 ,524 ,-67 ,-456 ,-709 ,-620 ,
-18 ,136 ,254 ,327 ,58 ,-152 ,59 ,-376 ,
-967 ,-761 ,-382 ,274 ,646 ,1018 ,789 ,55 ,
-490 ,-316 ,744 ,843 ,323 ,399 ,152 ,-21 ,
268 ,-3 ,-149 ,-674 ,-816 ,-671 ,-577 ,-244 ,
137 ,380 ,-319 ,-279 ,157 ,151 ,664 ,524 ,
-214 ,-585 ,-295 ,409 ,589 ,940 ,753 ,121 ,
-570 ,-622 ,-153 ,14 ,296 ,429 ,79 ,-82 ,
-340 ,-224 ,-208 ,-208 ,57 ,21 ,138 ,610 ,
483 ,263 ,-632 ,-637 ,-134 ,140 ,744 ,746 ,
385 ,-234 ,-444 ,-432 ,-500 ,13 ,70 ,-54 ,
-87 ,-74 ,4 ,-202 ,25 ,164 ,81 ,204 ,
130 ,87 ,42 ,-44 ,-28 ,100 ,105 ,127 ,
-107 ,-146 ,-295 ,-281 ,8 ,28 ,97 ,140 ,
-214 ,-258 ,-250 ,-43 ,99 ,247 ,274 ,179 ,
29 ,-75 ,-72 ,-44 ,74 ,224 ,388 ,439 ,
96 ,-196 ,-394 ,-394 ,-309 ,11 ,139 ,59 ,
-83 ,-131 ,-86 ,-158 ,108 ,250 ,209 ,333 ,
-49 ,23 ,-105 ,-77 ,36 ,72 ,271 ,280 ,
194 ,0 ,-221 ,-293 ,-349 ,-220 ,9 ,136 ,
56 ,-135 ,-227 ,-162 ,-84 ,7 ,155 ,207 ,
202 ,101 ,50 ,-44 ,-91 ,-43 ,-13 ,94 ,
232 ,165 ,59 ,-12 ,-91 ,-205 ,-250 ,-253 ,
42 ,153 ,193 ,130 ,41 ,-44 ,-144 ,-134 ,
99 ,249 ,307 ,303 ,256 ,132 ,27 ,-33 ,
-20 ,21 ,-5 ,-32 ,-15 ,-76 ,-145 ,-155 ,
-167 ,-89 ,-53 ,92 ,127 ,54 ,64 ,-30 ,
51 ,66 ,112 ,185 ,196 ,211 ,91 ,-23 ,
-11 ,-13 ,63 ,27 ,27 ,-28 ,-139 ,-124 ,
-223 ,-126 ,-112 ,-48 ,138 ,135 ,47 ,16 ,
-6 ,-9 ,29 ,165 ,200 ,228 ,238 ,140 ,
-47 ,-136 ,-56 ,-41 ,-42 ,18 ,-41 ,-82 ,
-152 ,-132 ,-101 ,-123 ,-29 ,48 ,98 ,199 ,
95 ,46 ,-7 ,14 ,45 ,86 ,217 ,236 ,
133 ,10 ,-64 ,-38 ,-60 ,-35 ,6 ,12 ,
-15 ,-71 ,-88 ,-123 ,-79 ,-19 ,22 ,83 ,
62 ,122 ,81 ,47 ,58 ,9 ,47 ,134 ,
122 ,68 ,-29 ,-57 ,-82 ,-80 ,-28 ,-19 ,
11 ,-10 ,-32 ,-50 ,-65 ,-15 ,26 ,57 ,
122 ,119 ,106 ,57 ,13 ,-7 ,2 ,39 ,
117 ,94 ,62 ,14 ,-30 ,-34 ,-46 ,-68 ,
-12 ,-3 ,12 ,10 ,5 ,-3 ,-45 ,-46 ,
22 ,79 ,115 ,132 ,126 ,71 ,20 ,-16 ,
-2 ,23 ,27 ,34 ,20 ,-10 ,-25 ,-45 ,
-16 ,-36 ,-20 ,-1 ,12 ,11 ,18 ,20 ,
1 ,-5 ,16 ,31 ,69 ,72 ,55 ,43 ,
12 ,12 ,24 ,39 ,40 ,8 ,-11 ,-34 ,
-41 ,-29 ,12 ,35 ,26 ,25 ,15 ,-9 ,
-2 ,8 ,46 ,55 ,72 ,52 ,38 ,39 ,
-49 ,-41 ,-13 ,16 ,44 ,52 ,24 ,0 ,
-41 ,-37 ,-27 ,-7 ,-1 ,11 ,10 ,5 ,
18 ,15 ,28 ,11 ,-18 ,-10 ,-5 ,9 ,
49 ,51 ,30 ,-15 ,-26 ,-23 ,-23 ,8 ,
27 ,39 ,34 ,7 ,21 ,0 ,5 ,-1 ,
-5 ,30 ,30 ,44 ,37 ,15 ,-1 ,-27 ,
4 ,17 ,28 ,38 ,13 ,11 ,30 ,19 ,
12 ,-3 ,7 ,6 ,21 ,54 ,46 ,32 ,
2 ,-10 ,-2 ,8 ,32 ,36 ,36 ,23 ,
-13 ,-6 ,-2 ,11 ,23 ,18 ,18 ,9 ,
18 ,21 ,27 ,25 ,18 ,4 ,-5 ,-3 ,
20 ,5 ,7 ,-1 ,0 ,5 ,10 ,12 ,
22 ,27 ,6 ,-5 ,13 ,5 ,8 ,14 ,
25 ,24 ,10 ,15 ,22 ,19 ,21 ,19 ,
13 ,14 ,10 ,24 ,15 ,21 ,8 ,-3 ,
-8 ,1 ,9 ,9 ,9 ,19 ,7 ,13 ,
19 ,15 ,22 ,14 ,17 ,20 ,17 ,36 ,
12 ,7 ,-7 ,-3 ,-2 ,-17 ,-2 ,13 ,
13 ,1 ,-12 ,-13 ,-18 ,-9 ,17 ,13 ,
32 ,23 ,16 ,22 ,1 ,-2 ,-4 ,10 ,
37 ,46 ,44 ,21 ,5 ,-3 ,-17 ,-10 ,
18 ,21 ,15 ,-2 ,-10 ,-18 ,-8 ,-2 ,
33 ,29 ,24 ,11 ,2 ,-2 ,17 ,19 ,
4 ,-7 ,-5 ,9 ,17 ,15 ,9 ,-9 ,
-1 ,-1 ,14 ,9 ,8 ,5 ,-10 ,-10 ,
-7 ,-1 ,2 ,-8 ,2 ,1 ,10 ,11 ,
9 ,11 ,-4 ,-10 ,1 ,12 ,24 ,22 ,
-3 ,-9 ,-13 ,-5 ,6 ,10 ,17 ,21 ,
5 ,-3 ,-3 ,-12 ,-6 ,-8 ,3 ,6 ,
9 ,14 ,-2 ,-8 ,-8 ,-13 ,-7 ,18 ,
19 ,15 ,0 ,-3 ,1 ,-3 ,-2 ,-3 ,
4 ,-5 ,-5 ,5 ,2 ,12 ,2 ,3 ,
-9 ,-5 ,10 ,13 ,8 ,9 ,-6 ,-3 ,
-5 ,13 ,9 ,11 ,1 ,6 ,-2 ,8 ,
10 ,12 ,6 ,3 ,-15 ,-7 ,-11 ,3 ,
-6 ,-14 ,-16 ,-4 ,-3 ,0 ,-3 ,-6 ,
-2 ,9 ,16 ,19 ,21 ,23 ,11 ,3 ,
-12 ,4 ,21 ,11 ,24 ,13 ,9 ,3 ,
-8 ,-15 ,-1 ,-1 ,-2 ,7 ,10 ,12 ,
7 ,-16 ,-13 ,-7 ,2 ,14 ,28 ,19 ,
7 ,-5 ,-11 ,-9 ,16 ,18 ,20 ,4 ,
-17 ,-7 ,-6 ,-15 ,-5 ,-2 ,4 ,4 ,
-5 ,1 ,-5 ,6 ,-1 ,4 ,5 ,8 ,
11 ,6 ,3 ,-2 ,6 ,15 ,25 ,13 ,
5 ,-1 ,-2 ,-1 ,-5 ,1 ,13 ,10 ,
-5 ,-13 ,-13 ,-9 ,7 ,-5 ,7 ,0 ,
5 ,4 ,12 ,10 ,11 ,3 ,-8 ,-5 ,
9 ,9 ,12 ,2 ,-7 ,-8 ,-13 ,-9 ,
4 ,0 ,6 ,1 ,-1 ,-1 ,-8 ,-4 ,
7 ,6 ,13 ,15 ,18 ,2 ,5 ,1 ,
-3 ,4 ,-4 ,7 ,10 ,9 ,1 ,-7 ,
-3 ,-3 ,0 ,5 ,1 ,6 ,4 ,-4 ,
1 ,5 ,5 ,7 ,10 ,9 ,7 ,9 ,
3 ,2 ,0 ,1 ,4 ,-1 ,3 ,0 ,
-11 ,-7 ,-10 ,2 ,5 ,4 ,7 ,-4 ,
1 ,1 ,3 ,3 ,-1 ,-1 ,-2 ,-6 ,
15 ,11 ,17 ,6 ,3 ,-4 ,3 ,0 ,
-1 ,7 ,2 ,5 ,3 ,-7 ,-9 ,-4 ,
-3 ,5 ,2 ,10 ,7 ,4 ,2 ,-1 ,
3 ,3 ,4 ,3 ,1 ,2 ,1 ,6 ,
6 ,5 ,7 ,-1 ,2 ,-2 ,3 ,-1 ,
6 ,2 ,-1 ,-2 ,-7 ,-9 ,-3 ,-2 ,
4 ,6 ,4 ,3 ,0 ,-5 ,-5 ,-1 ,
0 ,9 ,8 ,11 ,0 ,7 ,-9 ,8 ,
0 ,-2 ,6 ,5 ,1 ,4 ,-5 ,0 ,
-7 ,0 ,-4 ,1 ,2 ,4 ,0 ,0 ,
4 ,-1 ,5 ,0 ,6 ,6 ,9 ,5 ,
-4 ,0 ,-6 ,-3 ,1 ,3 ,3 ,7 ,
3 ,-1 ,-1 ,-2 ,-1 ,3 ,3 ,4 ,
0 ,-2 ,-1 ,0 ,-4 ,3 ,-1 ,-2 ,
-3 ,9 ,-4 ,4 ,-1 ,0 ,4 ,1 ,
2 ,1 ,4 ,-2 ,3 ,0 ,5 ,2 ,
5 ,-2 ,1 ,-4 ,-2 ,-1 ,1 ,3 ,
1 ,-1 ,2 ,-6 ,2 ,-5 ,1 ,1 ,
4 ,3 ,3 ,2 ,1 ,-3 ,1 ,-2 ,
0 ,3 ,3 ,1 ,4 ,-5 ,3 ,-4 ,
-1 ,2 ,1 ,2 ,2 ,3 ,1 ,2 ,
1 ,-1 ,1 ,2 ,3 ,2 ,-1 ,2 ,
1 ,0 ,0 ,4 ,1 ,3 ,-2 ,2 ,
1 ,-2 ,-2 ,1 ,-3 ,4 ,-2 ,2 ,
-1 ,1 ,-2 ,-1 ,-2 ,0 ,-1 ,3 ,
2 ,-3 ,0 ,-1 ,-2 ,1 ,0 ,-1 ,
-4 ,6 ,-4 ,1 ,0 ,-3 ,2 ,-3 ,
0 ,3 ,1 ,2 ,-2 ,2 ,-1 ,1 ,
-1 ,-1 ,0 ,-2 ,2 ,-1 ,2 ,1 ,
-1 ,1 ,-5 ,5 ,-3 ,3 ,1 ,-3 ,
-4 ,3 ,-2 ,1 ,-2 ,2 ,-4 ,3 ,
2 ,0 ,-1 ,2 ,-1 ,1 ,-3 ,2 ,
5 ,0 ,0 ,3 ,-3 ,1 ,-3 ,0 ,
2 ,-3 ,3 ,-4 ,1 ,1 ,-3 ,3 ,
2 ,-3 ,3 ,-2 ,1 ,2 ,-1 ,2 ,
1 ,1 ,2 ,1 ,1 ,1 ,-2 ,2 ,
2 ,-1 ,1 ,-1 ,-1 ,-1 ,-2 ,1 ,
3 ,-2 ,1 ,-1 ,-1 ,2 ,-1 ,1 ,
1 ,-1 ,1 ,-3 ,2 ,-1 ,3 ,1 ,
-1 ,-1 ,1 ,-1 ,1 ,-1 ,-1 ,0 ,
1 ,-4 ,4 ,-2 ,3 ,-1 ,0 ,1 ,
2 ,-3 ,3 ,-1 ,3 ,-2 ,1 ,0 ,
3 ,-4 ,3 ,-3 ,3 ,0 ,0 ,-1 ,
-1 ,-2 ,0 ,1 ,-1 ,3 ,-1 ,1 ,
0 ,1 ,-1 ,4 ,-2 ,4 ,-1 ,0 ,
2 ,0 ,1 ,-1 ,-2 ,0 ,1 ,-3 ,
-3 ,2 ,-1 ,-2 ,0 ,-2 ,2 ,-3 ,
-3 ,1 ,0 ,-1 ,3 ,-4 ,5 ,-2 ,
2 ,-2 ,1 ,2 ,-2 ,1 ,-2 ,-1 ,
-2 ,2 ,-1 ,-2 ,1 ,-4 ,1 ,-1 ,
-1 ,3 ,-1 ,1 ,-1 ,1 ,-1 ,2 ,
0 ,0 ,1 ,-1 ,3 ,-1 ,0 ,1 ,
4 ,-7 ,5 ,-2 ,3 ,1 ,-1 ,1 ,
1 ,-3 ,2 ,-1 ,2 ,-2 ,1 ,0 ,
1 ,-1 ,0 ,-1 ,-2 ,1 ,-1 ,2 ,
0 ,1 ,1 ,-1 ,-1 ,0 ,-1 ,2 ,
0 ,1 ,0 ,0 ,0 ,-2 ,1 ,-1 ,
0 ,0 ,0 ,0 ,1 ,-3 ,2 ,-4 ,
-2 ,4 ,-4 ,2 ,-1 ,-2 ,2 ,-2 ,
-1 ,1 ,-2 ,0 ,0 ,0 ,0 ,1 ,
-1 ,2 ,-2 ,1 ,-1 ,-1 ,2 ,0 ,
-1 ,1 ,-1 ,1 ,-1 ,-1 ,1 ,0 ,
2 ,-2 ,2 ,-4 ,3 ,-5 ,3 ,-3 ,
-1 ,1 ,0 ,0 ,-2 ,4 ,-5 ,4 ,
-1 ,1 ,0 ,0 ,4 ,-3 ,4 ,-5 ,
-6 ,5 ,-4 ,3 ,0 ,1 ,0 ,1 ,
1 ,0 ,0 ,0 ,0 ,-1 ,2 ,-3 ,
-4 ,3 ,-2 ,-1 ,-1 ,-1 ,0 ,1 ,
1 ,-2 ,2 ,-2 ,2 ,-1 ,1 ,1 ,
0 ,0 ,0 ,0 ,1 ,-2 ,0 ,-1 ,
0 ,1 ,-3 ,3 ,-3 ,1 ,0 ,0 ,
1 ,-2 ,2 ,0 ,-1 ,2 ,-2 ,2 ,
-2 ,4 ,-3 ,2 ,-1 ,1 ,0 ,-1 ,
-2 ,1 ,-2 ,1 ,0 ,0 ,-1 ,-1 ,
-2 ,1 ,0 ,0 ,2 ,-1 ,0 ,1 ,
3 ,-4 ,5 ,-6 ,6 ,-4 ,3 ,-1 ,
1 ,-1 ,1 ,-1 ,1 ,-1 ,1 ,-1 ,
-1 ,0 ,-1 ,2 ,-2 ,3 ,-2 ,1 ,
0 ,1 ,0 ,-1 ,2 ,-2 ,1 ,-1 ,
2 ,-2 ,2 ,-2 ,1 ,0 ,-1 ,1 ,
0 ,2 ,-3 ,2 ,-1 ,-1 ,2 ,-1 ,
1 ,0 ,-1 ,2 ,-3 ,4 ,-4 ,5 ,
2 ,-2 ,2 ,-3 ,4 ,-5 ,4 ,-2 ,
2 ,-3 ,2 ,-1 ,0 ,1 ,-1 ,1 ,
2 ,-3 ,2 ,0 ,-1 ,1 ,-1 ,1 ,
0 ,0 ,0 ,0 ,-1 ,2 ,-3 ,4 ,
3 ,-1 ,-1 ,2 ,-3 ,3 ,-2 ,1 ,
2 ,-3 ,3 ,-3 ,3 ,-1 ,-1 ,2 ,
4 ,-4 ,4 ,-4 ,4 ,-3 ,2 ,-1 ,
1 ,-2 ,2 ,-1 ,0 ,1 ,-1 ,0 ,
-2 ,3 ,-3 ,2 ,-2 ,2 ,-2 ,2 ,
1 ,1 ,-2 ,2 ,-2 ,1 ,1 ,-2 ,
-2 ,1 ,1 ,-1 ,0 ,1 ,-3 ,4 ,
1 ,0 ,0 ,0 ,0 ,0 ,-1 ,2 ,
2 ,-2 ,2 ,-2 ,2 ,-1 ,0 ,1 ,
1 ,-2 ,2 ,-1 ,1 ,-1 ,0 ,0 ,
-2 ,3 ,-3 ,1 ,2 ,-4 ,5 ,-5 ,
-3 ,3 ,-3 ,2 ,-1 ,0 ,1 ,-2 ,
-3 ,2 ,-1 ,0 ,0 ,1 ,-1 ,1 ,
2 ,-2 ,3 ,-4 ,3 ,-1 ,0 ,1 ,
1 ,0 ,0 ,0 ,1 ,-2 ,2 ,-2 ,
0 ,-1 ,2 ,-2 ,1 ,0 ,-1 ,1 ,
0 ,0 ,0 ,-1 ,1 ,0 ,0 ,0 ,
0 ,-1 ,2 ,-2 ,2 ,-1 ,0 ,0 ,
1 ,-2 ,3 ,-4 ,4 ,-2 ,0 ,1 ,
1 ,-1 ,2 ,-3 ,4 ,-4 ,3 ,-2 ,
0 ,0 ,-1 ,1 ,-1 ,1 ,0 ,-1 ,
-1 ,1 ,-1 ,1 ,0 ,0 ,-1 ,1 ,
2 ,-2 ,2 ,-3 ,3 ,-1 ,-1 ,2 ,
1 ,0 ,-1 ,2 ,-2 ,2 ,-3 ,3 ,
4 ,-4 ,2 ,0 ,-1 ,1 ,-1 ,1 ,
4 ,-4 ,2 ,0 ,-1 ,1 ,0 ,-1 ,
0 ,-1 ,2 ,-3 ,4 ,-5 ,4 ,-2 ,
-3 ,4 ,-5 ,5 ,-4 ,3 ,-1 ,0 ,
0 ,0 ,1 ,-1 ,1 ,-1 ,1 ,-1 ,
2 ,-3 ,3 ,-3 ,2 ,0 ,-1 ,1 ,
1 ,0 ,-1 ,1 ,-1 ,2 ,-2 ,1 ,
1 ,0 ,0 ,0 ,-1 ,2 ,-2 ,1 ,
-3 ,4 ,-3 ,1 ,1 ,-2 ,2 ,-2 ,
-1 ,0 ,0 ,-1 ,2 ,-1 ,0 ,-1 ,
-2 ,2 ,-2 ,2 ,-2 ,2 ,-2 ,2 ,
2 ,-2 ,2 ,-1 ,0 ,1 ,-1 ,0 ,
-1 ,1 ,-1 ,1 ,-1 ,2 ,-3 ,2 ,
0 ,1 ,-1 ,1 ,-1 ,0 ,0 ,0 ,
-2 ,2 ,-1 ,-1 ,2 ,-2 ,2 ,-1 ,
0 ,0 ,0 ,1 ,-2 ,3 ,-4 ,4 ,
2 ,-1 ,0 ,1 ,-1 ,1 ,-1 ,1 ,
2 ,-3 ,3 ,-2 ,1 ,0 ,0 ,-1
};

static unsigned char max98090_reg_values[12][2] = {
	{0x00, 0x80},
	{0x03, 0x00},
	{0x04, 0x40},
	{0x05, 0x04},
	{0x1B, 0x10},
	{0x21, 0x81},
	{0x22, 0x04},
	{0x25, 0x01},
	{0x2E, 0x01},
	{0x2F, 0x02},
	{0x3F, 0x33},
	{0x45, 0x80}
};

static int max98090_i2c_write(u8 reg, void *buffer, size_t size)
{
	int status = -1;

	/* Switch to the I2C bus the gas gauge is connected to */
	if (select_bus(MAX98090_I2C_BUS, 100)) {
		printf("MAX98080: Failed to switch to bus %d\n", MAX98090_I2C_BUS);
		goto done;
	}

	status = i2c_write(MAX98090_I2C_ADDR, reg, 1, buffer, size);
	if (status)
		printf("MAX98090: I2C write failed: %d\n", status);
done:
	/* Switch back to the original I2C bus */
	select_bus(0, 100);

	return status;
}

static int max98090_init(void)
{
	unsigned char reg,val;
	int indx;

	for(indx = 0; indx < 12; indx++) {
		reg = max98090_reg_values[indx][0];
		val = max98090_reg_values[indx][1];
		max98090_i2c_write(reg, &val, 1);
	}

	return 0;
}

static void max98090_disable(void)
{
	unsigned char val = 0x00;

	max98090_i2c_write(0x45, &val, 1);
}

static int omap_mcbsp_control_module(struct omap_mcbsp *mcbsp, int flag)
{
	unsigned int val, reg;
	switch (mcbsp->id) {
		case 1:
			reg = OMAP4_CM1_ABE_MCBSP1_CLKCTRL;
			break;
		case 2:
			reg = OMAP4_CM1_ABE_MCBSP2_CLKCTRL;
			break;
		case 3:
			reg = OMAP4_CM1_ABE_MCBSP3_CLKCTRL;
			break;
		default:
			return -ENODEV;
			break;
	}

	val = __raw_readl(reg);

	if(flag)
		val |= 0x02;
	else
		val &= ~0x03;

	__raw_writel(val, reg);

	val = __raw_readl(reg);

	return 0;
}

static int omap_mcbsp_init(struct omap_mcbsp *mcbsp)
{
	MCBSP_WRITE(mcbsp, SYSCONFIG, 0x00000208);
	MCBSP_WRITE(mcbsp, THRSH2,    0x00000000);
	MCBSP_WRITE(mcbsp, THRSH1,    0x00000000);
	MCBSP_WRITE(mcbsp, XCCR,      0x00001008);
	MCBSP_WRITE(mcbsp, RCCR,      0x00000808);
	MCBSP_WRITE(mcbsp, RCR2,      0x00000000);
	MCBSP_WRITE(mcbsp, RCR1,      0x00000000);
	MCBSP_WRITE(mcbsp, XCR2,      0x00008041);
	MCBSP_WRITE(mcbsp, XCR1,      0x00000040);
	MCBSP_WRITE(mcbsp, SRGR2,     0x0000001f);
	MCBSP_WRITE(mcbsp, SRGR1,     0x00000f00);
	MCBSP_WRITE(mcbsp, PCR0,      0x0000000f);
	MCBSP_WRITE(mcbsp, SPCR2,     0x00000200);
	MCBSP_WRITE(mcbsp, SPCR1,     0x00000000);
	MCBSP_WRITE(mcbsp, SPCR2,     0x00000201);

	return 0;
}

static int omap_mcbsp_idle(struct omap_mcbsp *mcbsp)
{
	MCBSP_WRITE(mcbsp, SPCR2,     0x00000000);
	MCBSP_WRITE(mcbsp, SYSCONFIG, 0x00000000);
}

static int wait_until_ready(struct omap_mcbsp *mcbsp)
{
	int count = 1000;

	while (count && ((MCBSP_READ(mcbsp, SPCR2) & XRDY) == 0)) {
		udelay(1);
		count--;
	}

	return (count) ? 0 : -ETIMEDOUT;
}

static int omap_mcbsp_xfer(struct omap_mcbsp *mcbsp)
{
	int bytes;

	for (bytes = 0; bytes < sizeof(click) / 2; bytes++) {
		/* write left sample */
		wait_until_ready(mcbsp);
		MCBSP_WRITE(mcbsp, DXR, click[bytes]);
		/* write right sample */
		wait_until_ready(mcbsp);
		MCBSP_WRITE(mcbsp, DXR, click[bytes]);
	}
	return 0;
}

static void inline auxclk0_enable(void)
{
	__raw_writel((1 << 16) | (1 << 8), 0x4a30a310);
}

static void inline auxclk0_disable(void)
{
	__raw_writel(0x00, 0x4a30a310);
}

int play_click(void)
{
	struct omap_mcbsp *mcbsp;

	mcbsp = omap_mcbsp_get_instance(OMAP_MCBSP_2);
	if(mcbsp == NULL) {
		printf("Cannot get McBSP instance");
		return -ENODEV;
	}

	auxclk0_enable();
	max98090_init();
	/* enable McBSP module */
	omap_mcbsp_control_module(mcbsp, 1);
	omap_mcbsp_init(mcbsp);
	omap_mcbsp_xfer(mcbsp);
	max98090_disable();
	/* disable McBSP module */
	omap_mcbsp_idle(mcbsp);
	omap_mcbsp_control_module(mcbsp, 0);
	auxclk0_disable();
	return 0;
}
