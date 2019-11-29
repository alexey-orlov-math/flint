/*
    Copyright (C) 2019 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpz_mod.h"

/*
    An entry of {a, b, c, d, e, f, g, h} gives a prime
        2^a * 5^b * 7^c * 11^d * 13^e * 17^f * 19^g * 23^h + 1
*/

static const unsigned char _eight_primes[8] = {2, 5, 7, 11, 13, 17, 19, 23};

static const unsigned char _smooth_prime_lut[][8] =
{
{0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.844100*2^16 = 33857 */
{0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}, /*0x.895100*2^16 = 35153 */
{0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00}, /*0x.9df100*2^16 = 40433 */
{0x03, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00}, /*0x.b63900*2^16 = 46649 */
{0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.cea500*2^16 = 52901 */
{0x04, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}, /*0x.840880*2^17 = 67601 */
{0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00}, /*0x.8d1d80*2^17 = 72251 */
{0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00}, /*0x.91e580*2^17 = 74699 */
{0x06, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}, /*0x.992080*2^17 = 78401 */
{0x04, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00}, /*0x.e1c880*2^17 = 115601 */
{0x06, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00}, /*0x.8fb040*2^18 = 147137 */
{0x05, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.998840*2^18 = 157217 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}, /*0x.9d0c40*2^18 = 160817 */
{0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00}, /*0x.a30f40*2^18 = 166973 */
{0x07, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00}, /*0x.a66040*2^18 = 170369 */
{0x03, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.865720*2^19 = 275129 */
{0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00}, /*0x.8d1d20*2^19 = 289001 */
{0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00}, /*0x.b04520*2^19 = 361001 */
{0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.ca8220*2^19 = 414737 */
{0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00}, /*0x.cbc4a0*2^19 = 417317 */
{0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03}, /*0x.82b350*2^20 = 535349 */
{0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00}, /*0x.8d0410*2^20 = 577601 */
{0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.b131d0*2^20 = 725789 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00}, /*0x.b65190*2^20 = 746777 */
{0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.cea410*2^20 = 846401 */
{0x05, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.865708*2^21 = 1100513 */
{0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00}, /*0x.912298*2^21 = 1188947 */
{0x05, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00}, /*0x.91e508*2^21 = 1195169 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00}, /*0x.a32088*2^21 = 1336337 */
{0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}, /*0x.be1c08*2^21 = 1557377 */
{0x08, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00}, /*0x.816404*2^22 = 2119937 */
{0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}, /*0x.9485e4*2^22 = 2433401 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02}, /*0x.954c44*2^22 = 2446097 */
{0x04, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00}, /*0x.b06444*2^22 = 2890001 */
{0x03, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00}, /*0x.bbd264*2^22 = 3077273 */
{0x04, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00}, /*0x.81cda2*2^23 = 4253393 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04}, /*0x.88a422*2^23 = 4477457 */
{0x07, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00}, /*0x.91e502*2^23 = 4780673 */
{0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03}, /*0x.9a76c2*2^23 = 5061473 */
{0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.ca8202*2^23 = 6635777 */
{0x06, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00}, /*0x.d24282*2^23 = 6889793 */
{0x04, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.95eed1*2^24 = 9826001 */
{0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00}, /*0x.97210d*2^24 = 9904397 */
{0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00}, /*0x.a30f01*2^24 = 10686209 */
{0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03}, /*0x.a65881*2^24 = 10901633 */
{0x08, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00}, /*0x.bec901*2^24 = 12503297 */
{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05}, /*0x.c46bef*2^24 = 12872687 */
{0x02, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00}, /*0x.8120a2*2^25 = 16924997 */
{0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.a17020*2^25 = 21160001 */
{0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00}, /*0x.a4b385*2^25 = 21587723 */
{0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x02}, /*0x.ae9c80*2^25 = 22886657 */
{0x08, 0x00, 0x03, 0x00, 0x00, 0x02, 0x00, 0x00}, /*0x.c19b80*2^25 = 25376513 */
{0x07, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.eb1840*2^25 = 30814337 */
{0x02, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02}, /*0x.ec5c82*2^25 = 30980357 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03}, /*0x.860abe*2^26 = 35138297 */
{0x07, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}, /*0x.861820*2^26 = 35152001 */
{0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00}, /*0x.8f97f4*2^26 = 37642193 */
{0x06, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x00}, /*0x.9b0350*2^26 = 40635713 */
{0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03}, /*0x.cbf0c9*2^26 = 53461799 */
{0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00}, /*0x.e5f027*2^26 = 60276893 */
{0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02}, /*0x.e68aa1*2^26 = 60435077 */
{0x06, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00}, /*0x.848a68*2^27 = 69489473 */
{0x04, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00}, /*0x.99414e*2^27 = 80349809 */
{0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.9b0b88*2^27 = 81288257 */
{0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00}, /*0x.a8081c*2^27 = 88096997 */
{0x07, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02}, /*0x.abe630*2^27 = 90124673 */
{0x05, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00}, /*0x.c30d4c*2^27 = 102263393 */
{0x07, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00}, /*0x.cab590*2^27 = 106278017 */
{0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04}, /*0x.d58072*2^27 = 111936401 */
{0x08, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.95eed0*2^28 = 157216001 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00}, /*0x.9d5379*2^28 = 164968337 */
{0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00}, /*0x.a38803*2^28 = 171475001 */
{0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04}, /*0x.b468b2*2^28 = 189172517 */
{0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04}, /*0x.de0ab4*2^28 = 232827713 */
{0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03}, /*0x.dee093*2^28 = 233703737 */
{0x04, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03}, /*0x.f71ae5*2^28 = 259108433 */
{0x06, 0x00, 0x02, 0x00, 0x00, 0x04, 0x00, 0x00}, /*0x.f9c9c4*2^28 = 261921857 */
{0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00}, /*0x.85d1dc*2^29 = 280640387 */
{0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00}, /*0x.8b4d71*2^29 = 292138529 */
{0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00}, /*0x.8d8018*2^29 = 296747777 */
{0x03, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00}, /*0x.9103b8*2^29 = 304117529 */
{0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04}, /*0x.9a4148*2^29 = 323496197 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04}, /*0x.a242e6*2^29 = 340286657 */
{0x07, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00}, /*0x.b27abc*2^29 = 374298497 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00}, /*0x.e608d4*2^29 = 482417297 */
{0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00}, /*0x.861924*2^30 = 562448657 */
{0x06, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02}, /*0x.87aa17*2^30 = 569017793 */
{0x07, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04}, /*0x.912e62*2^30 = 608934017 */
{0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00}, /*0x.aa8471*2^30 = 715201649 */
{0x05, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03}, /*0x.cbf0c9*2^30 = 855388769 */
{0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04}, /*0x.d13b51*2^30 = 877581377 */
{0x09, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00}, /*0x.d880c8*2^30 = 908079617 */
{0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02}, /*0x.e68aa1*2^30 = 966961217 */
{0x09, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03}, /*0x.fb00f8*2^30 = 1052786177 */
{0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00}, /*0x.a0931c*2^31 = 1346997857 */
{0x03, 0x00, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00}, /*0x.a3d804*2^31 = 1374421577 */
{0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00}, /*0x.a8081c*2^31 = 1409551937 */
{0x05, 0x00, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00}, /*0x.b18cde*2^31 = 1489399649 */
{0x08, 0x00, 0x05, 0x00, 0x00, 0x00, 0x02, 0x00}, /*0x.b928fe*2^31 = 1553235713 */
{0x09, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00}, /*0x.bb6a84*2^31 = 1572160001 */
{0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04}, /*0x.bbe1ac*2^31 = 1576064513 */
{0x06, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00}, /*0x.f568c2*2^31 = 2058641729 */
{0x03, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00}, /*0x.f8e977*2^31 = 2088025001 */
{0x05, 0x00, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00}, /*0x.8932ac*2^32 = 2301799457 */
{0x07, 0x05, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00}, /*0x.a38803*2^32 = 2743600001 */
{0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03}, /*0x.a5b3a3*2^32 = 2780013497 */
{0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00}, /*0x.aa8471*2^32 = 2860806593 */
{0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04}, /*0x.b19b54*2^32 = 2979746969 */
{0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00}, /*0x.bf3e7a*2^32 = 3208542737 */
{0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03}, /*0x.cbf0c9*2^32 = 3421555073 */
{0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00}, /*0x.e5f027*2^32 = 3857721089 */
{0x08, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03}, /*0x.f71ae5*2^32 = 4145734913 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02}, /*0x.837e04*2^33 = 4412147777 */
{0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00}, /*0x.9533e3*2^33 = 5006411537 */
{0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x04, 0x00}, /*0x.a56bf6*2^33 = 5550632033 */
{0x06, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0x00}, /*0x.bf8a7b*2^33 = 6427047617 */
{0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00}, /*0x.939645*2^34 = 9904396001 */
{0x06, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00}, /*0x.a94298*2^34 = 11358856001 */
{0x08, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00}, /*0x.c7212c*2^34 = 13363360001 */
{0x07, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00}, /*0x.d4087e*2^34 = 14229305729 */
{0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00}, /*0x.91c5d6*2^35 = 19565295377 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x02}, /*0x.9c25a4*2^35 = 20957701937 */
{0x0b, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00}, /*0x.a4b385*2^35 = 22105827329 */
{0x03, 0x00, 0x00, 0x00, 0x03, 0x05, 0x00, 0x00}, /*0x.b9eea9*2^35 = 24955406633 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x00}, /*0x.88953e*2^36 = 36663714497 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00}, /*0x.8e2ef9*2^36 = 38167092497 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02}, /*0x.a88b14*2^36 = 45242991617 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00}, /*0x.dddab5*2^36 = 59553569297 */
{0x0b, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00}, /*0x.808c76*2^37 = 69014050817 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02}, /*0x.837e04*2^37 = 70594364417 */
{0x06, 0x00, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00}, /*0x.e14906*2^37 = 120949098689 */
{0x07, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x03}, /*0x.f247ed*2^37 = 130073600897 */
{0x05, 0x00, 0x00, 0x05, 0x04, 0x00, 0x00, 0x00}, /*0x.891584*2^38 = 147192883553 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06}, /*0x.8d2d93*2^38 = 151588750337 */
{0x05, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x00}, /*0x.a9b974*2^38 = 182240227937 */
{0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x05, 0x00}, /*0x.aa9c4e*2^38 = 183191708417 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x02}, /*0x.be4501*2^38 = 204300384017 */
{0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x05, 0x00}, /*0x.83b9db*2^39 = 282879454157 */
{0x03, 0x00, 0x00, 0x04, 0x00, 0x03, 0x00, 0x02}, /*0x.8dc0da*2^39 = 304412978057 */
{0x08, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00}, /*0x.91c5d6*2^39 = 313044726017 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x05}, /*0x.a475be*2^39 = 353175013097 */
{0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00}, /*0x.b12d9e*2^39 = 380487276737 */
{0x05, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x02}, /*0x.84d120*2^40 = 570444263777 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04}, /*0x.87dbb3*2^40 = 583506543377 */
{0x0b, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04}, /*0x.b19b54*2^40 = 762815223809 */
{0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00}, /*0x.b441b3*2^40 = 774196400849 */
{0x09, 0x00, 0x00, 0x00, 0x05, 0x03, 0x00, 0x00}, /*0x.d97505*2^40 = 933971204609 */
{0x08, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00}, /*0x.843c07*2^41 = 1135885600001 */
{0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00}, /*0x.888697*2^41 = 1172747210753 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04}, /*0x.e4d056*2^41 = 1965495725057 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05}, /*0x.eb99b5*2^41 = 2023792202177 */
{0x08, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05}, /*0x.ff4f49*2^41 = 2193093768449 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00}, /*0x.9a307b*2^42 = 2648953372337 */
{0x07, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03}, /*0x.a0981a*2^42 = 2758986583937 */
{0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00}, /*0x.b441b3*2^42 = 3096785603393 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04}, /*0x.b905ed*2^42 = 3178673621897 */
{0x0c, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00}, /*0x.91c5d6*2^43 = 5008715616257 */
{0x02, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04}, /*0x.9d3f29*2^43 = 5402956229477 */
{0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06}, /*0x.b780fc*2^43 = 6305144584289 */
{0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x04}, /*0x.c188d0*2^43 = 6649792282433 */
{0x09, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x05}, /*0x.e64708*2^43 = 7912273686017 */
{0x0e, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x00}, /*0x.88953e*2^44 = 9385910910977 */
{0x08, 0x00, 0x00, 0x03, 0x00, 0x03, 0x03, 0x00}, /*0x.a71693*2^44 = 11482212704513 */
{0x09, 0x00, 0x05, 0x00, 0x00, 0x05, 0x00, 0x00}, /*0x.b1cc15*2^44 = 12218130738689 */
{0x02, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00}, /*0x.b71992*2^44 = 12582528518597 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x00}, /*0x.c09a74*2^44 = 13235600933057 */
{0x03, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x03}, /*0x.81f434*2^45 = 17860731323033 */
{0x0c, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04}, /*0x.824fa5*2^45 = 17909824000001 */
{0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x05}, /*0x.8a7e19*2^45 = 19034274390017 */
{0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x03}, /*0x.aac163*2^45 = 23468446238849 */
{0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x04}, /*0x.afd1ec*2^45 = 24164518848809 */
{0x07, 0x00, 0x00, 0x00, 0x07, 0x03, 0x00, 0x00}, /*0x.8f8e40*2^46 = 39460283394689 */
{0x09, 0x00, 0x00, 0x02, 0x00, 0x03, 0x04, 0x00}, /*0x.904dad*2^46 = 39665825706497 */
{0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x05}, /*0x.949185*2^46 = 40838184409049 */
{0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x04}, /*0x.994991*2^46 = 42135314605889 */
{0x0c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x02}, /*0x.be4501*2^46 = 52300898308097 */
{0x08, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0x00}, /*0x.d70414*2^46 = 59103129876737 */
{0x06, 0x04, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00}, /*0x.8980d2*2^47 = 75593186680001 */
{0x06, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x00}, /*0x.928519*2^47 = 80550180747713 */
{0x09, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04}, /*0x.9aeed1*2^47 = 85175254422017 */
{0x0c, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04}, /*0x.ae23b2*2^47 = 95734170259457 */
{0x07, 0x00, 0x00, 0x00, 0x04, 0x03, 0x03, 0x00}, /*0x.e016dc*2^47 = 123194394084737 */
{0x09, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x03}, /*0x.f990cc*2^47 = 137200147242497 */
{0x05, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04}, /*0x.9eb652*2^48 = 174505901433953 */
{0x0c, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00}, /*0x.b441b3*2^48 = 198194278617089 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x00}, /*0x.c09a74*2^48 = 211769614928897 */
{0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00}, /*0x.d60e05*2^48 = 235355705857793 */
{0x08, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05}, /*0x.f158f3*2^48 = 265364345982209 */
{0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x06}, /*0x.f61ae6*2^48 = 270595393646657 */
{0x07, 0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00}, /*0x.9269d5*2^49 = 321966500707457 */
{0x05, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x04}, /*0x.9acde5*2^49 = 340418212612193 */
{0x05, 0x00, 0x00, 0x04, 0x00, 0x05, 0x00, 0x02}, /*0x.a006b7*2^49 = 351901402632737 */
{0x0b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x03}, /*0x.aac163*2^49 = 375495139821569 */
{0x07, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x04}, /*0x.afd1ec*2^49 = 386632301580929 */
{0x09, 0x00, 0x00, 0x03, 0x00, 0x04, 0x03, 0x00}, /*0x.b187fc*2^49 = 390395231953409 */
{0x0a, 0x00, 0x00, 0x04, 0x00, 0x02, 0x04, 0x00}, /*0x.80633a*2^50 = 564654695351297 */
{0x06, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x04}, /*0x.995f05*2^50 = 674533564142657 */
{0x05, 0x00, 0x00, 0x00, 0x04, 0x05, 0x00, 0x02}, /*0x.9c1601*2^50 = 686473325633057 */
{0x0d, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05}, /*0x.af8682*2^50 = 771969006493697 */
{0x08, 0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00}, /*0x.e4b76a*2^50 = 1005905670912257 */
{0x0b, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x00}, /*0x.f57d1b*2^50 = 1079670712526849 */
{0x04, 0x00, 0x03, 0x00, 0x00, 0x04, 0x05, 0x00}, /*0x.810779*2^51 = 1134952780009553 */
{0x0a, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x04}, /*0x.89d83e*2^51 = 1212494798242817 */
{0x04, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00}, /*0x.979a60*2^51 = 1333514367843857 */
{0x08, 0x00, 0x00, 0x00, 0x07, 0x04, 0x00, 0x00}, /*0x.988724*2^51 = 1341649635419393 */
{0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x05}, /*0x.ab341a*2^51 = 1505922190729217 */
{0x09, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00}, /*0x.e24678*2^51 = 1990338368009729 */
{0x08, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x04}, /*0x.82fcfd*2^52 = 2304369439220993 */
{0x09, 0x00, 0x00, 0x00, 0x06, 0x05, 0x00, 0x00}, /*0x.c775a6*2^52 = 3508929815712257 */
{0x07, 0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x03}, /*0x.d32cd5*2^52 = 3715032115190657 */
{0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x05}, /*0x.e20acb*2^52 = 3976575784894337 */
{0x0d, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x00}, /*0x.f57d1b*2^52 = 4318682850107393 */
{0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x06}, /*0x.f61ae6*2^52 = 4329526298346497 */
{0x0a, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07}, /*0x.83e4b5*2^53 = 4640586414035969 */
{0x0a, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x04}, /*0x.86205f*2^53 = 4719155235859457 */
{0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x04}, /*0x.91be0f*2^53 = 5127855503188289 */
{0x09, 0x00, 0x00, 0x04, 0x00, 0x05, 0x00, 0x02}, /*0x.a006b7*2^53 = 5630422442123777 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00, 0x02}, /*0x.a5d761*2^53 = 5835023267880977 */
{0x05, 0x00, 0x03, 0x00, 0x00, 0x04, 0x00, 0x05}, /*0x.a7b2d0*2^53 = 5900366165444129 */
{0x0a, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04}, /*0x.c4964c*2^53 = 6916793801245697 */
{0x02, 0x00, 0x00, 0x00, 0x00, 0x05, 0x04, 0x03}, /*0x.fff2d5*2^53 = 9005389675632797 */
{0x0a, 0x00, 0x00, 0x00, 0x05, 0x06, 0x00, 0x00}, /*0x.826a76*2^54 = 9177201056478209 */
{0x0e, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00, 0x00}, /*0x.a08308*2^54 = 11295016684896257 */
{0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07}, /*0x.a9eba6*2^54 = 11957093243378177 */
{0x0f, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04}, /*0x.b905ed*2^54 = 13019847155286017 */
{0x08, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04}, /*0x.cc2799*2^54 = 14366108924559617 */
{0x0e, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00}, /*0x.d60e05*2^54 = 15062765174898689 */
{0x0a, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x04}, /*0x.ff86a2*2^54 = 17981038331696129 */
{0x03, 0x00, 0x00, 0x04, 0x00, 0x05, 0x04, 0x00}, /*0x.99ff05*2^55 = 21673035298913417 */
{0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x04}, /*0x.9f4671*2^55 = 22415987370332417 */
{0x06, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04}, /*0x.a61283*2^55 = 23372600161000001 */
{0x07, 0x00, 0x03, 0x00, 0x00, 0x04, 0x00, 0x05}, /*0x.a7b2d0*2^55 = 23601464661776513 */
{0x04, 0x00, 0x00, 0x04, 0x05, 0x00, 0x00, 0x04}, /*0x.acf206*2^55 = 24339902201771729 */
{0x0d, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00}, /*0x.cca41b*2^55 = 28800667630329857 */
{0x05, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05}, /*0x.81de59*2^56 = 36554746651804001 */
{0x09, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x03}, /*0x.83933c*2^56 = 37035109568375297 */
{0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x07, 0x00}, /*0x.9049b5*2^56 = 40613441463082337 */
{0x07, 0x00, 0x00, 0x00, 0x04, 0x03, 0x05, 0x00}, /*0x.9e001e*2^56 = 44473176264589697 */
{0x0b, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06}, /*0x.ad77ef*2^56 = 48827039660726273 */
{0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x05}, /*0x.b06ccf*2^56 = 49659232241402369 */
{0x0d, 0x00, 0x00, 0x00, 0x06, 0x05, 0x00, 0x00}, /*0x.c775a6*2^56 = 56142877051396097 */
{0x0a, 0x00, 0x03, 0x00, 0x00, 0x04, 0x05, 0x00}, /*0x.810779*2^57 = 72636977920611329 */
{0x09, 0x00, 0x00, 0x05, 0x00, 0x00, 0x07, 0x00}, /*0x.82ee0d*2^57 = 73706975968096769 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x06, 0x03, 0x00}, /*0x.8664b5*2^57 = 75656757267288497 */
{0x06, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04}, /*0x.8d2955*2^57 = 79466840547400001 */
{0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x03, 0x04}, /*0x.9aea8e*2^57 = 87210089490338657 */
{0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x04}, /*0x.ad11b2*2^57 = 97429254560577473 */
{0x08, 0x00, 0x00, 0x03, 0x00, 0x05, 0x00, 0x04}, /*0x.f07e74*2^57 = 135386066903794433 */
{0x08, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04}, /*0x.819081*2^58 = 145876635844000001 */
{0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x06}, /*0x.84014d*2^58 = 148624519960425809 */
{0x05, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x05}, /*0x.90be60*2^58 = 162966878976323489 */
{0x0b, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x03}, /*0x.9a628e*2^58 = 173822035526957057 */
{0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00}, /*0x.ceb3de*2^58 = 232726447479951617 */
{0x04, 0x00, 0x00, 0x00, 0x03, 0x06, 0x00, 0x04}, /*0x.d2e3bb*2^58 = 237440562208387409 */
{0x07, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x03}, /*0x.e501b1*2^58 = 257838525449696897 */
{0x0b, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x03}, /*0x.f0e48c*2^58 = 271221147877881857 */
{0x05, 0x00, 0x05, 0x00, 0x00, 0x04, 0x00, 0x05}, /*0x.8064e7*2^59 = 289117942106762273 */
{0x0a, 0x00, 0x05, 0x00, 0x00, 0x05, 0x00, 0x03}, /*0x.8408ea*2^59 = 297315993395233793 */
{0x07, 0x00, 0x00, 0x04, 0x00, 0x05, 0x04, 0x00}, /*0x.99ff05*2^59 = 346768564782614657 */
{0x06, 0x00, 0x00, 0x00, 0x04, 0x04, 0x05, 0x00}, /*0x.a7e020*2^59 = 378021998249012417 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x03}, /*0x.a9d925*2^59 = 382464196812169337 */
{0x08, 0x00, 0x00, 0x04, 0x05, 0x00, 0x00, 0x04}, /*0x.acf206*2^59 = 389438435228347649 */
{0x0a, 0x00, 0x00, 0x04, 0x00, 0x03, 0x00, 0x05}, /*0x.d28947*2^59 = 474085466112941057 */
{0x0c, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x05}, /*0x.e42616*2^59 = 513745373821554689 */
{0x0b, 0x00, 0x00, 0x05, 0x00, 0x00, 0x03, 0x04}, /*0x.8c9301*2^60 = 633090104031987713 */
{0x09, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x07}, /*0x.8fb8b5*2^60 = 647264181602801153 */
{0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05}, /*0x.94ffc8*2^60 = 671032524882400001 */
{0x06, 0x00, 0x00, 0x00, 0x05, 0x03, 0x00, 0x05}, /*0x.a6d946*2^60 = 751419878122533569 */
{0x02, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00, 0x04}, /*0x.ab5908*2^60 = 771681827177259077 */
{0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x07}, /*0x.c9c7d6*2^60 = 908739086496741377 */
{0x08, 0x00, 0x00, 0x00, 0x05, 0x04, 0x04, 0x00}, /*0x.e5b96f*2^60 = 1034586521523612929 */
{0x05, 0x00, 0x00, 0x05, 0x00, 0x04, 0x05, 0x00}, /*0x.eca7e8*2^60 = 1065803382934800929 */
{0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04}, /*0x.87146c*2^61 = 1216690499828859137 */
{0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x05}, /*0x.8b1613*2^61 = 1252777462003947857 */
{0x06, 0x00, 0x00, 0x00, 0x08, 0x06, 0x00, 0x00}, /*0x.8be775*2^61 = 1260144420067663937 */
{0x04, 0x00, 0x00, 0x06, 0x05, 0x00, 0x04, 0x00}, /*0x.984566*2^61 = 1371536150082683729 */
{0x09, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06}, /*0x.a3fb5f*2^61 = 1477017949736969729 */
{0x03, 0x00, 0x00, 0x00, 0x05, 0x04, 0x00, 0x05}, /*0x.b146da*2^61 = 1596767241010383833 */
{0x0a, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x04}, /*0x.bd23a6*2^61 = 1703615040145263617 */
{0x04, 0x00, 0x00, 0x05, 0x00, 0x00, 0x04, 0x05}, /*0x.eff6f1*2^61 = 2161409183296708049 */
{0x09, 0x00, 0x00, 0x04, 0x00, 0x06, 0x00, 0x03}, /*0x.f46a41*2^61 = 2201495174870396417 */
{0x09, 0x00, 0x06, 0x00, 0x04, 0x05, 0x00, 0x00}, /*0x.879957*2^62 = 2442734224193675777 */
{0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x05}, /*0x.953920*2^62 = 2688165230588571137 */
{0x0a, 0x00, 0x06, 0x00, 0x00, 0x04, 0x00, 0x04}, /*0x.9c4e51*2^62 = 2815757349213684737 */
{0x0a, 0x00, 0x00, 0x04, 0x00, 0x04, 0x05, 0x00}, /*0x.ac1d06*2^62 = 3100518932173966337 */
{0x0a, 0x00, 0x03, 0x00, 0x00, 0x05, 0x00, 0x05}, /*0x.b22dfd*2^62 = 3209799194001605633 */
{0x04, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x05}, /*0x.b7a892*2^62 = 3308497009961476433 */
{0x05, 0x00, 0x00, 0x00, 0x05, 0x06, 0x00, 0x03}, /*0x.c1b28f*2^62 = 3489343914192823649 */
{0x04, 0x00, 0x00, 0x00, 0x04, 0x05, 0x00, 0x05}, /*0x.e7d2cf*2^62 = 4176160476488696177 */
{0x0c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04}, /*0x.ecd4d8*2^62 = 4266375778652655617 */
{0x08, 0x00, 0x07, 0x00, 0x00, 0x04, 0x00, 0x04}, /*0x.88c487*2^63 = 4927575361123948289 */
{0x0a, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0x03}, /*0x.89e678*2^63 = 4968381033816251393 */
{0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x05, 0x03}, /*0x.a177b1*2^63 = 5817481730458786217 */
{0x09, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x05}, /*0x.ae850c*2^63 = 6287735805954059777 */
{0x0b, 0x00, 0x05, 0x00, 0x00, 0x05, 0x04, 0x00}, /*0x.b0c74a*2^63 = 6369116063986235393 */
{0x07, 0x00, 0x00, 0x00, 0x00, 0x05, 0x04, 0x04}, /*0x.b7f689*2^63 = 6627966801265737857 */
{0x08, 0x00, 0x00, 0x05, 0x04, 0x00, 0x00, 0x05}, /*0x.d25c89*2^63 = 7579071085597842689 */
{0x0a, 0x00, 0x00, 0x00, 0x05, 0x04, 0x00, 0x04}, /*0x.f6a55c*2^63 = 8886356819536049153 */
{0x07, 0x00, 0x00, 0x06, 0x04, 0x05, 0x00, 0x00}, /*0x.ff3b45*2^63 = 9195685226705650817 */
{0x07, 0x00, 0x03, 0x00, 0x00, 0x05, 0x00, 0x06}, /*0x.80110e*2^64 = 9228172682754616193 */
{0x07, 0x00, 0x00, 0x03, 0x05, 0x00, 0x00, 0x06}, /*0x.81f468*2^64 = 9364224192536177537 */
{0x0a, 0x00, 0x00, 0x05, 0x06, 0x00, 0x00, 0x03}, /*0x.86689d*2^64 = 9685164563070211073 */
{0x0b, 0x00, 0x05, 0x00, 0x00, 0x06, 0x00, 0x03}, /*0x.8c4978*2^64 = 10108743775437948929 */
{0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0x05}, /*0x.8e7a9f*2^64 = 10266693620659363229 */
{0x0a, 0x00, 0x00, 0x04, 0x04, 0x06, 0x00, 0x00}, /*0x.8f6f97*2^64 = 10335646205222880257 */
{0x05, 0x00, 0x07, 0x00, 0x00, 0x05, 0x00, 0x04}, /*0x.9150cf*2^64 = 10471097642388390113 */
{0x0a, 0x00, 0x05, 0x00, 0x04, 0x06, 0x00, 0x00}, /*0x.a4a7ea*2^64 = 11864709088940710913 */
{0x05, 0x00, 0x04, 0x00, 0x00, 0x05, 0x00, 0x06}, /*0x.e01dd8*2^64 = 16149302194820578337 */
{0x08, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x04}, /*0x.e3578a*2^64 = 16381714095359126273 */
{0x08, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0x04}, /*0x.e58808*2^64 = 16539478967835678977 */
{0x0d, 0x00, 0x00, 0x08, 0x06, 0x08, 0x00, 0x07}, /*0x.977400*2^128 = 201315753943344863644036821430284967937 */
{0x03, 0x00, 0x00, 0x00, 0x07, 0x08, 0x08, 0x07}, /*0x.985698*2^128 = 202492286286810790437922936221068336153 */
{0x0e, 0x00, 0x00, 0x00, 0x05, 0x07, 0x06, 0x09}, /*0x.9f2141*2^128 = 211519923313957068893521212124359344129 */
{0x08, 0x07, 0x00, 0x00, 0x00, 0x08, 0x07, 0x09}, /*0x.a8fc1b*2^128 = 224619314500089695434264067444740000001 */
{0x08, 0x00, 0x00, 0x00, 0x09, 0x06, 0x06, 0x08}, /*0x.b59f3c*2^128 = 241417060812417193281164455701879284993 */
{0x14, 0x00, 0x00, 0x00, 0x00, 0x07, 0x09, 0x09}, /*0x.bc233f*2^128 = 250077889582264191015188456592240541697 */
{0x09, 0x00, 0x00, 0x07, 0x00, 0x08, 0x06, 0x08}, /*0x.c0e8fb*2^128 = 256421491496774145772985886547254564353 */
{0x0b, 0x00, 0x00, 0x08, 0x09, 0x07, 0x00, 0x06}, /*0x.d4c040*2^128 = 282794572515043903725031263863386236929 */
{0x0c, 0x00, 0x00, 0x00, 0x07, 0x06, 0x09, 0x06}, /*0x.def31f*2^128 = 296350979539355403848369839560486350849 */
{0x0e, 0x00, 0x00, 0x08, 0x00, 0x06, 0x06, 0x08}, /*0x.eaf691*2^128 = 312319602099877160249449937939908673537 */
{0x0a, 0x00, 0x00, 0x07, 0x00, 0x09, 0x07, 0x06}, /*0x.eb938f*2^128 = 313134751430843285764364617598348674049 */
{0x0e, 0x00, 0x00, 0x07, 0x00, 0x07, 0x08, 0x06}, /*0x.f7cdb5*2^128 = 329387420190229615475317798442553622529 */
{0x17, 0x00, 0x00, 0x00, 0x0d, 0x0b, 0x07, 0x0a}, /*0x.838004*2^192 = 3224372305314505999731230563592619434528670607112207859713 */
{0x1c, 0x00, 0x00, 0x00, 0x07, 0x0b, 0x0a, 0x0b}, /*0x.898840*2^192 = 3372280456223566487919284082022261011830810717167059730433 */
{0x17, 0x00, 0x00, 0x00, 0x07, 0x0b, 0x08, 0x0e}, /*0x.90daa6*2^192 = 3551812353780482466976621314574519540421180228165825462273 */
{0x1c, 0x00, 0x00, 0x0a, 0x00, 0x0a, 0x0a, 0x0a}, /*0x.916542*2^192 = 3565088486727419991008074270946581990658772639631336800257 */
{0x1a, 0x00, 0x00, 0x00, 0x0b, 0x07, 0x0d, 0x09}, /*0x.987331*2^192 = 3738062417125879757367726246205088567592814768935128793089 */
{0x18, 0x00, 0x00, 0x09, 0x0a, 0x06, 0x08, 0x09}, /*0x.a439ca*2^192 = 4026803574740221083962824220953216561664085820467721535489 */
{0x18, 0x00, 0x00, 0x0a, 0x0a, 0x0c, 0x0b, 0x00}, /*0x.a60cb0*2^192 = 4071523423234022089688349550455788710099661020875672518657 */
{0x1b, 0x00, 0x00, 0x0d, 0x00, 0x0a, 0x0a, 0x08}, /*0x.b6e99c*2^192 = 4485002623661810971674618955226749177284334955906719547393 */
{0x18, 0x00, 0x00, 0x00, 0x09, 0x08, 0x09, 0x0e}, /*0x.bd5860*2^192 = 4642731312024884673910820696559650822020113866330334035969 */
{0x19, 0x00, 0x00, 0x09, 0x0a, 0x08, 0x07, 0x08}, /*0x.d936c3*2^192 = 5326069716704457177415360182404940898493916714485910863873 */
{0x1a, 0x00, 0x00, 0x07, 0x00, 0x0a, 0x0c, 0x0b}, /*0x.e2c01b*2^192 = 5559904152009347893564996369584799073711455527208675704833 */
{0x17, 0x00, 0x00, 0x0d, 0x00, 0x0b, 0x09, 0x09}, /*0x.eb425c*2^192 = 5768539558722921348436763195702825422099259762366866259969 */
};

static const slong num_entries = sizeof(_smooth_prime_lut)/sizeof(const unsigned char [8]);

static void _get_lut_entry(fmpz_t p, slong i)
{
    slong j;
    fmpz_one(p);
    for (j = 0; j < 8; j++)
    {
        fmpz_t t;
        fmpz_init_set_ui(t, _eight_primes[j]);
        fmpz_pow_ui(t, t, _smooth_prime_lut[i][j]);
        fmpz_mul(p, p, t);
        fmpz_clear(t);
    }
    fmpz_add_ui(p, p, 1);
}

int fmpz_next_smooth_prime(fmpz_t a, const fmpz_t b)
{
    int success;
    slong lo, mid, hi;
    fmpz_t lo_p, mid_p, hi_p;
    fmpz_init(lo_p);
    fmpz_init(mid_p);
    fmpz_init(hi_p);

    FLINT_ASSERT(num_entries > 1);

    lo = 0;
    hi = num_entries - 1;
    _get_lut_entry(lo_p, lo);
    _get_lut_entry(hi_p, hi);

    if (fmpz_cmp(b, lo_p) < 0)
    {
        fmpz_swap(a, lo_p);
        success = 1;
        goto cleanup;
    }
    if (fmpz_cmp(hi_p, b) <= 0)
    {
        fmpz_zero(a);
        success = 0;
        goto cleanup;        
    }

    /* lo_p <= b < hi_p now */

    while (hi - lo > 4)
    {
        mid = lo + (hi - lo)/2;
        _get_lut_entry(mid_p, mid);
        if (fmpz_cmp(b, mid_p) < 0)
        {
            hi = mid;
            fmpz_swap(hi_p, mid_p);
        }
        else
        {
            lo = mid;
            fmpz_swap(lo_p, mid_p);
        }
    }

    for (; lo <= hi; lo++)
    {
        _get_lut_entry(lo_p, lo);
        if (fmpz_cmp(lo_p, b) > 0)
        {
            fmpz_swap(a, lo_p);
            success = 1;
            goto cleanup;
        }
    }

    success = 0;
    fmpz_zero(a);

cleanup:

    fmpz_clear(lo_p);
    fmpz_clear(mid_p);
    fmpz_clear(hi_p);

    return success;
}