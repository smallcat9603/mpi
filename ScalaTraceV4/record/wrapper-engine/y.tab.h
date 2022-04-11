/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ASNDIV = 258,
    ASNMINUS = 259,
    ASNMULT = 260,
    ASNOP = 261,
    ASNPLUS = 262,
    ASTERICK = 263,
    ATTR = 264,
    BOOT = 265,
    CLR = 266,
    COLON = 267,
    COMMA = 268,
    COMMENT = 269,
    DIV = 270,
    ELLIPSIS = 271,
    EQ = 272,
    EXTRAFIELD = 273,
    GE = 274,
    GT = 275,
    INCR = 276,
    INTEGER = 277,
    LBRACE = 278,
    LBRACKET = 279,
    LE = 280,
    LPAREN = 281,
    LT = 282,
    MINUS = 283,
    MOD = 284,
    NL = 285,
    NOSAVE = 286,
    PRE = 287,
    POST = 288,
    PERIOD = 289,
    PLUS = 290,
    POUND = 291,
    RBRACE = 292,
    RBRACKET = 293,
    RPAREN = 294,
    SAVE = 295,
    SEMI = 296,
    SET = 297,
    T_CHAR = 298,
    T_CONST = 299,
    T_DOUBLE = 300,
    T_EXTERN = 301,
    T_INT = 302,
    T_STRING = 303,
    T_STRUCT = 304,
    T_VOID = 305,
    T_VOLATILE = 306,
    VERIFY = 307,
    VCODE = 308,
    VCODE_G_PRE_B = 309,
    VCODE_G_PRE_E = 310,
    VCODE_G_POST_B = 311,
    VCODE_G_POST_E = 312,
    VCODE_L_PRE_B = 313,
    VCODE_L_PRE_E = 314,
    VCODE_L_POST_B = 315,
    VCODE_L_POST_E = 316,
    WRAPPER_PRE_B = 317,
    WRAPPER_PRE_E = 318,
    WRAPPER_POST_B = 319,
    WRAPPER_POST_E = 320,
    DOUBLE = 321,
    ID = 322,
    QSTRING = 323
  };
#endif
/* Tokens.  */
#define ASNDIV 258
#define ASNMINUS 259
#define ASNMULT 260
#define ASNOP 261
#define ASNPLUS 262
#define ASTERICK 263
#define ATTR 264
#define BOOT 265
#define CLR 266
#define COLON 267
#define COMMA 268
#define COMMENT 269
#define DIV 270
#define ELLIPSIS 271
#define EQ 272
#define EXTRAFIELD 273
#define GE 274
#define GT 275
#define INCR 276
#define INTEGER 277
#define LBRACE 278
#define LBRACKET 279
#define LE 280
#define LPAREN 281
#define LT 282
#define MINUS 283
#define MOD 284
#define NL 285
#define NOSAVE 286
#define PRE 287
#define POST 288
#define PERIOD 289
#define PLUS 290
#define POUND 291
#define RBRACE 292
#define RBRACKET 293
#define RPAREN 294
#define SAVE 295
#define SEMI 296
#define SET 297
#define T_CHAR 298
#define T_CONST 299
#define T_DOUBLE 300
#define T_EXTERN 301
#define T_INT 302
#define T_STRING 303
#define T_STRUCT 304
#define T_VOID 305
#define T_VOLATILE 306
#define VERIFY 307
#define VCODE 308
#define VCODE_G_PRE_B 309
#define VCODE_G_PRE_E 310
#define VCODE_G_POST_B 311
#define VCODE_G_POST_E 312
#define VCODE_L_PRE_B 313
#define VCODE_L_PRE_E 314
#define VCODE_L_POST_B 315
#define VCODE_L_POST_E 316
#define WRAPPER_PRE_B 317
#define WRAPPER_PRE_E 318
#define WRAPPER_POST_B 319
#define WRAPPER_POST_E 320
#define DOUBLE 321
#define ID 322
#define QSTRING 323

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 46 "parser.y"

  char * s;
  long i;
  double r;
  /*  array_ele_t * a;*/

#line 200 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
