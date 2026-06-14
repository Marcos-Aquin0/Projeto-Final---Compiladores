/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include "globals.h"
#include "asnt.h"
#include "symtab.h"

void yyerror(const char *s);
int yylex(); //lexer

ASTNode* root = NULL; // raiz da árvore sintática
int syntaxErrorCount = 0; // contador de erros sintáticos

#line 83 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUM = 3,                        /* NUM  */
  YYSYMBOL_ID = 4,                         /* ID  */
  YYSYMBOL_ELSE = 5,                       /* ELSE  */
  YYSYMBOL_IF = 6,                         /* IF  */
  YYSYMBOL_INT = 7,                        /* INT  */
  YYSYMBOL_RETURN = 8,                     /* RETURN  */
  YYSYMBOL_VOID = 9,                       /* VOID  */
  YYSYMBOL_WHILE = 10,                     /* WHILE  */
  YYSYMBOL_OPAREN = 11,                    /* OPAREN  */
  YYSYMBOL_CPAREN = 12,                    /* CPAREN  */
  YYSYMBOL_OKEYS = 13,                     /* OKEYS  */
  YYSYMBOL_CKEYS = 14,                     /* CKEYS  */
  YYSYMBOL_OBRACKT = 15,                   /* OBRACKT  */
  YYSYMBOL_CBRACKT = 16,                   /* CBRACKT  */
  YYSYMBOL_SUM = 17,                       /* SUM  */
  YYSYMBOL_SUB = 18,                       /* SUB  */
  YYSYMBOL_DIV = 19,                       /* DIV  */
  YYSYMBOL_MULT = 20,                      /* MULT  */
  YYSYMBOL_NQ = 21,                        /* NQ  */
  YYSYMBOL_BT = 22,                        /* BT  */
  YYSYMBOL_BTE = 23,                       /* BTE  */
  YYSYMBOL_LT = 24,                        /* LT  */
  YYSYMBOL_LTE = 25,                       /* LTE  */
  YYSYMBOL_EQ = 26,                        /* EQ  */
  YYSYMBOL_SMC = 27,                       /* SMC  */
  YYSYMBOL_COMMA = 28,                     /* COMMA  */
  YYSYMBOL_ATRIB = 29,                     /* ATRIB  */
  YYSYMBOL_IFX = 30,                       /* IFX  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_program = 32,                   /* program  */
  YYSYMBOL_decl_list = 33,                 /* decl_list  */
  YYSYMBOL_decl = 34,                      /* decl  */
  YYSYMBOL_var_decl = 35,                  /* var_decl  */
  YYSYMBOL_spec_type = 36,                 /* spec_type  */
  YYSYMBOL_func_decl = 37,                 /* func_decl  */
  YYSYMBOL_params = 38,                    /* params  */
  YYSYMBOL_param_list = 39,                /* param_list  */
  YYSYMBOL_param = 40,                     /* param  */
  YYSYMBOL_compound_decl = 41,             /* compound_decl  */
  YYSYMBOL_local_decl = 42,                /* local_decl  */
  YYSYMBOL_state_list = 43,                /* state_list  */
  YYSYMBOL_statement = 44,                 /* statement  */
  YYSYMBOL_expr_decl = 45,                 /* expr_decl  */
  YYSYMBOL_select_decl = 46,               /* select_decl  */
  YYSYMBOL_iter_decl = 47,                 /* iter_decl  */
  YYSYMBOL_return_decl = 48,               /* return_decl  */
  YYSYMBOL_expr = 49,                      /* expr  */
  YYSYMBOL_var = 50,                       /* var  */
  YYSYMBOL_simp_expr = 51,                 /* simp_expr  */
  YYSYMBOL_relational = 52,                /* relational  */
  YYSYMBOL_sum_expr = 53,                  /* sum_expr  */
  YYSYMBOL_sum = 54,                       /* sum  */
  YYSYMBOL_term = 55,                      /* term  */
  YYSYMBOL_mult = 56,                      /* mult  */
  YYSYMBOL_factor = 57,                    /* factor  */
  YYSYMBOL_activation = 58,                /* activation  */
  YYSYMBOL_args = 59,                      /* args  */
  YYSYMBOL_arg_list = 60                   /* arg_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   103

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  66
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  108

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   285


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    44,    44,    56,    57,    61,    62,    66,    74,    82,
      86,    87,    91,    96,   104,   105,   109,   110,   114,   122,
     134,   141,   142,   146,   147,   151,   152,   153,   154,   155,
     159,   168,   172,   175,   179,   186,   192,   193,   197,   198,
     202,   206,   215,   218,   222,   223,   224,   225,   226,   227,
     231,   232,   236,   237,   241,   242,   246,   247,   251,   252,
     253,   254,   262,   270,   271,   275,   276
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUM", "ID", "ELSE",
  "IF", "INT", "RETURN", "VOID", "WHILE", "OPAREN", "CPAREN", "OKEYS",
  "CKEYS", "OBRACKT", "CBRACKT", "SUM", "SUB", "DIV", "MULT", "NQ", "BT",
  "BTE", "LT", "LTE", "EQ", "SMC", "COMMA", "ATRIB", "IFX", "$accept",
  "program", "decl_list", "decl", "var_decl", "spec_type", "func_decl",
  "params", "param_list", "param", "compound_decl", "local_decl",
  "state_list", "statement", "expr_decl", "select_decl", "iter_decl",
  "return_decl", "expr", "var", "simp_expr", "relational", "sum_expr",
  "sum", "term", "mult", "factor", "activation", "args", "arg_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-44)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-16)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      75,   -44,   -44,    11,    75,   -44,   -44,    76,   -44,   -44,
     -44,   -23,    28,   -44,    39,    41,   -44,    24,    32,    53,
      46,    34,   -44,    51,   -44,   -44,    68,    32,    75,    58,
      75,    52,   -44,   -44,   -44,   -44,    77,     2,   -44,   -13,
     -44,     6,    27,    78,    23,    79,    38,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,    59,    62,   -44,    48,     4,
     -44,   -44,    38,    38,   -44,    38,   -44,    60,    38,    80,
     -44,    38,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
      38,    38,   -44,   -44,    38,   -44,    81,    66,    72,    83,
     -44,    84,   -44,   -44,   -44,    35,     4,   -44,   -44,    38,
     -44,    27,    27,   -44,    92,   -44,    27,   -44
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    10,    11,     0,     2,     4,     6,     0,     5,     1,
       3,     0,     0,     9,     0,     0,     7,    11,     0,     0,
       0,    14,    17,     0,    22,    13,    18,     0,     0,     0,
      24,     0,    12,    16,     8,    21,     0,     0,    19,     0,
      61,    40,     0,     0,     0,     0,     0,    20,    31,    26,
      23,    25,    27,    28,    29,     0,    59,    39,    43,    51,
      55,    60,    64,     0,    34,     0,    36,     0,     0,     0,
      30,     0,    52,    53,    44,    45,    46,    47,    48,    49,
       0,     0,    57,    56,     0,    66,     0,    63,     0,     0,
      37,     0,    58,    38,    59,    42,    50,    54,    62,     0,
      41,     0,     0,    65,    32,    35,     0,    33
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -44,   -44,   -44,    94,    69,    33,   -44,   -44,   -44,    73,
      -9,   -44,   -44,   -42,   -44,   -44,   -44,   -44,   -43,    -5,
     -44,   -44,    20,   -44,    21,   -44,    19,   -44,   -44,   -44
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,     5,     6,     7,     8,    20,    21,    22,
      49,    30,    37,    50,    51,    52,    53,    54,    55,    56,
      57,    80,    58,    81,    59,    84,    60,    61,    86,    87
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      64,    67,    15,    69,    13,    40,    41,    42,    43,    25,
      44,     9,    45,    46,    16,    24,    47,    62,    32,    85,
      88,    63,    89,    82,    83,    91,    40,    41,    93,    48,
      40,    41,    42,    43,    46,    44,   -15,    45,    46,    14,
      24,    40,    41,    15,    23,    24,     1,    19,    17,    46,
      66,    18,    72,    73,    48,    16,   103,    26,    27,   104,
     105,    19,    28,    36,   107,    72,    73,    29,    38,    74,
      75,    76,    77,    78,    79,    94,    94,    11,    11,    94,
      12,    39,     1,    31,     2,    34,    70,    90,   100,    65,
      68,    71,    92,    98,    99,   101,   102,   106,    10,    35,
      95,    33,    96,    97
};

static const yytype_int8 yycheck[] =
{
      42,    44,    15,    46,    27,     3,     4,     5,     6,    18,
       8,     0,    10,    11,    27,    13,    14,    11,    27,    62,
      63,    15,    65,    19,    20,    68,     3,     4,    71,    27,
       3,     4,     5,     6,    11,     8,    12,    10,    11,    11,
      13,     3,     4,    15,     3,    13,     7,    14,     9,    11,
      27,    12,    17,    18,    27,    27,    99,     4,    12,   101,
     102,    28,    28,    30,   106,    17,    18,    16,    16,    21,
      22,    23,    24,    25,    26,    80,    81,     1,     1,    84,
       4,     4,     7,    15,     9,    27,    27,    27,    16,    11,
      11,    29,    12,    12,    28,    12,    12,     5,     4,    30,
      80,    28,    81,    84
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     7,     9,    32,    33,    34,    35,    36,    37,     0,
      34,     1,     4,    27,    11,    15,    27,     9,    12,    36,
      38,    39,    40,     3,    13,    41,     4,    12,    28,    16,
      42,    15,    41,    40,    27,    35,    36,    43,    16,     4,
       3,     4,     5,     6,     8,    10,    11,    14,    27,    41,
      44,    45,    46,    47,    48,    49,    50,    51,    53,    55,
      57,    58,    11,    15,    44,    11,    27,    49,    11,    49,
      27,    29,    17,    18,    21,    22,    23,    24,    25,    26,
      52,    54,    19,    20,    56,    49,    59,    60,    49,    49,
      27,    49,    12,    49,    50,    53,    55,    57,    12,    28,
      16,    12,    12,    49,    44,    44,     5,    44
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    33,    33,    34,    34,    35,    35,    35,
      36,    36,    37,    37,    38,    38,    39,    39,    40,    40,
      41,    42,    42,    43,    43,    44,    44,    44,    44,    44,
      45,    45,    46,    46,    46,    47,    48,    48,    49,    49,
      50,    50,    51,    51,    52,    52,    52,    52,    52,    52,
      53,    53,    54,    54,    55,    55,    56,    56,    57,    57,
      57,    57,    58,    59,    59,    60,    60
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     3,     6,     3,
       1,     1,     6,     5,     1,     1,     3,     1,     2,     4,
       4,     2,     0,     2,     0,     1,     1,     1,     1,     1,
       2,     1,     5,     7,     2,     5,     2,     3,     3,     1,
       1,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     4,     1,     0,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: decl_list  */
#line 44 "parser.y"
                                { 
        (yyval.node) = createNode(NODE_PROGRAM, (yyvsp[0].node), NULL, NULL, yylineno, NULL);
        root = (yyval.node);
    }
#line 1201 "parser.tab.c"
    break;

  case 3: /* decl_list: decl_list decl  */
#line 56 "parser.y"
                               { (yyval.node) = createNode(NODE_DECL_LIST, (yyvsp[-1].node), (yyvsp[0].node), NULL, yylineno, NULL); }
#line 1207 "parser.tab.c"
    break;

  case 4: /* decl_list: decl  */
#line 57 "parser.y"
                               { (yyval.node) = createNode(NODE_DECL_LIST, (yyvsp[0].node), NULL, NULL, yylineno, NULL); }
#line 1213 "parser.tab.c"
    break;

  case 5: /* decl: func_decl  */
#line 61 "parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1219 "parser.tab.c"
    break;

  case 6: /* decl: var_decl  */
#line 62 "parser.y"
                             { (yyval.node) = (yyvsp[0].node); }
#line 1225 "parser.tab.c"
    break;

  case 7: /* var_decl: spec_type ID SMC  */
#line 66 "parser.y"
                              { 
        //printf("DEBUG: Declarando variável simples %s do tipo %s\n", $2, $1->value);
        (yyval.node) = createNode(NODE_VAR_DECL, (yyvsp[-2].node), NULL, (yyvsp[-1].sval), yylineno, (yyvsp[-2].node)->value); 
        (yyval.node)->scope = current_scope(); 
        (yyval.node)->isArray = 0; 
        (yyval.node)->arraySize = 0; 
        (yyval.node)->value = (yyvsp[-1].sval);  // Importante: armazenar o nome da variável
    }
#line 1238 "parser.tab.c"
    break;

  case 8: /* var_decl: spec_type ID OBRACKT NUM CBRACKT SMC  */
#line 74 "parser.y"
                                           {
        //printf("DEBUG: Declarando vetor %s do tipo %s com tamanho %d\n", $2, $1->value, $4);
        (yyval.node) = createNode(NODE_VAR_DECL, (yyvsp[-5].node), NULL, (yyvsp[-4].sval), yylineno, (yyvsp[-5].node)->value);
        (yyval.node)->scope = current_scope();
        (yyval.node)->isArray = 1;  // Indica que é um vetor
        (yyval.node)->arraySize = (yyvsp[-2].ival);  // Armazena o tamanho do vetor
        (yyval.node)->value = (yyvsp[-4].sval);  // Importante: armazenar o nome da variável
    }
#line 1251 "parser.tab.c"
    break;

  case 9: /* var_decl: spec_type error SMC  */
#line 82 "parser.y"
                              { yyerror("Erro na declaração de variável"); (yyval.node) = NULL; }
#line 1257 "parser.tab.c"
    break;

  case 10: /* spec_type: INT  */
#line 86 "parser.y"
                              { (yyval.node) = createNode(NODE_SPEC_TYPE, NULL, NULL, "int", yylineno, NULL); }
#line 1263 "parser.tab.c"
    break;

  case 11: /* spec_type: VOID  */
#line 87 "parser.y"
                              { (yyval.node) = createNode(NODE_SPEC_TYPE, NULL, NULL, "void", yylineno, NULL); }
#line 1269 "parser.tab.c"
    break;

  case 12: /* func_decl: spec_type ID OPAREN params CPAREN compound_decl  */
#line 91 "parser.y"
                                                    {
        ASTNode* idNode = createNode(NODE_FUNC, NULL, NULL, (yyvsp[-4].sval), yylineno, (yyvsp[-5].node)->value);
        (yyval.node) = createNode(NODE_FUNC_DECL, createNode(NODE_DECL, (yyvsp[-5].node), idNode, NULL, yylineno, NULL), 
                       createNode(NODE_DECL, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL), (yyvsp[-4].sval), yylineno, (yyvsp[-5].node)->value);
    }
#line 1279 "parser.tab.c"
    break;

  case 13: /* func_decl: spec_type ID OPAREN CPAREN compound_decl  */
#line 96 "parser.y"
                                               {
        ASTNode* idNode = createNode(NODE_FUNC, NULL, NULL, (yyvsp[-3].sval), yylineno, (yyvsp[-4].node)->value);
        (yyval.node) = createNode(NODE_FUNC_DECL, createNode(NODE_DECL, (yyvsp[-4].node), idNode, NULL, yylineno, NULL), 
                       createNode(NODE_DECL, NULL, (yyvsp[0].node), NULL, yylineno, NULL), (yyvsp[-3].sval), yylineno, (yyvsp[-4].node)->value);
    }
#line 1289 "parser.tab.c"
    break;

  case 14: /* params: param_list  */
#line 104 "parser.y"
                              { (yyval.node) = createNode(NODE_PARAMS, (yyvsp[0].node), NULL, NULL, yylineno, NULL); }
#line 1295 "parser.tab.c"
    break;

  case 15: /* params: VOID  */
#line 105 "parser.y"
                              { (yyval.node) = createNode(NODE_PARAMS, NULL, NULL, "void", yylineno, NULL); }
#line 1301 "parser.tab.c"
    break;

  case 16: /* param_list: param_list COMMA param  */
#line 109 "parser.y"
                              { (yyval.node) = createNode(NODE_PARAM_LIST, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL); }
#line 1307 "parser.tab.c"
    break;

  case 17: /* param_list: param  */
#line 110 "parser.y"
                              { (yyval.node) = createNode(NODE_PARAM_LIST, (yyvsp[0].node), NULL, NULL, yylineno, NULL); }
#line 1313 "parser.tab.c"
    break;

  case 18: /* param: spec_type ID  */
#line 114 "parser.y"
                             { 
        //printf("DEBUG: Declarando parâmetro simples %s\n", $2);
        (yyval.node) = createNode(NODE_PARAM, (yyvsp[-1].node), NULL, (yyvsp[0].sval), yylineno, (yyvsp[-1].node)->value); 
        (yyval.node)->scope = current_scope();
        (yyval.node)->isArray = 0;
        (yyval.node)->arraySize = 0;
        (yyval.node)->value = (yyvsp[0].sval);
    }
#line 1326 "parser.tab.c"
    break;

  case 19: /* param: spec_type ID OBRACKT CBRACKT  */
#line 122 "parser.y"
                                   {
        //printf("DEBUG: Declarando parâmetro array %s[]\n", $2);
        (yyval.node) = createNode(NODE_PARAM, (yyvsp[-3].node), NULL, (yyvsp[-2].sval), yylineno, (yyvsp[-3].node)->value);
        (yyval.node)->scope = current_scope();
        (yyval.node)->isArray = 1;  // Marca como array
        (yyval.node)->arraySize = -1;  // -1 indica array sem tamanho específico (parâmetro de função)
        (yyval.node)->value = (yyvsp[-2].sval);  // Nome do parâmetro
        (yyval.node)->idType = (yyvsp[-3].node)->value;  // Tipo do parâmetro (int, void, etc)
    }
#line 1340 "parser.tab.c"
    break;

  case 20: /* compound_decl: OKEYS local_decl state_list CKEYS  */
#line 134 "parser.y"
                                      {
        (yyval.node) = createNode(NODE_COMPOUND_DECL, (yyvsp[-2].node), (yyvsp[-1].node), NULL, yylineno, NULL);
        (yyval.node)->scope = current_scope();  // Define o escopo do bloco composto
    }
#line 1349 "parser.tab.c"
    break;

  case 21: /* local_decl: local_decl var_decl  */
#line 141 "parser.y"
                             { (yyval.node) = createNode(NODE_LOCAL_DECL, (yyvsp[-1].node), (yyvsp[0].node), NULL, yylineno, NULL); }
#line 1355 "parser.tab.c"
    break;

  case 22: /* local_decl: %empty  */
#line 142 "parser.y"
                             { (yyval.node) = NULL; }
#line 1361 "parser.tab.c"
    break;

  case 23: /* state_list: state_list statement  */
#line 146 "parser.y"
                             { (yyval.node) = createNode(NODE_STATE_LIST, (yyvsp[-1].node), (yyvsp[0].node), NULL, yylineno, NULL); }
#line 1367 "parser.tab.c"
    break;

  case 24: /* state_list: %empty  */
#line 147 "parser.y"
                             { (yyval.node) = NULL; }
#line 1373 "parser.tab.c"
    break;

  case 25: /* statement: expr_decl  */
#line 151 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1379 "parser.tab.c"
    break;

  case 26: /* statement: compound_decl  */
#line 152 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1385 "parser.tab.c"
    break;

  case 27: /* statement: select_decl  */
#line 153 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1391 "parser.tab.c"
    break;

  case 28: /* statement: iter_decl  */
#line 154 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1397 "parser.tab.c"
    break;

  case 29: /* statement: return_decl  */
#line 155 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1403 "parser.tab.c"
    break;

  case 30: /* expr_decl: expr SMC  */
#line 159 "parser.y"
                           { 
        // Verifica se a expressão é uma atribuição ou uma chamada de função
        if ((yyvsp[-1].node)->type == NODE_EXPR || ((yyvsp[-1].node)->type == NODE_ACTIVATION)) {
            (yyval.node) = createNode(NODE_EXPR_DECL, (yyvsp[-1].node), NULL, NULL, yylineno, NULL);
        } else {
            yyerror("Expressão inválida: uso de vetor sem atribuição ou chamada de função");
            (yyval.node) = NULL;
        }
    }
#line 1417 "parser.tab.c"
    break;

  case 31: /* expr_decl: SMC  */
#line 168 "parser.y"
                           { (yyval.node) = NULL; }
#line 1423 "parser.tab.c"
    break;

  case 32: /* select_decl: IF OPAREN expr CPAREN statement  */
#line 172 "parser.y"
                                              {
        (yyval.node) = createNode(NODE_SELECT_DECL, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL);
    }
#line 1431 "parser.tab.c"
    break;

  case 33: /* select_decl: IF OPAREN expr CPAREN statement ELSE statement  */
#line 175 "parser.y"
                                                     {
        (yyval.node) = createNode(NODE_SELECT_DECL, (yyvsp[-4].node), 
            createNode(NODE_STATEMENT, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL), NULL, yylineno, NULL);
    }
#line 1440 "parser.tab.c"
    break;

  case 34: /* select_decl: ELSE statement  */
#line 179 "parser.y"
                     {
        yyerror("Bloco else sem precedência de um bloco if");
        (yyval.node) = NULL;
    }
#line 1449 "parser.tab.c"
    break;

  case 35: /* iter_decl: WHILE OPAREN expr CPAREN statement  */
#line 186 "parser.y"
                                       {
        (yyval.node) = createNode(NODE_ITER_DECL, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL);
    }
#line 1457 "parser.tab.c"
    break;

  case 36: /* return_decl: RETURN SMC  */
#line 192 "parser.y"
                           { (yyval.node) = createNode(NODE_RETURN_DECL, NULL, NULL, NULL, yylineno, NULL); }
#line 1463 "parser.tab.c"
    break;

  case 37: /* return_decl: RETURN expr SMC  */
#line 193 "parser.y"
                           { (yyval.node) = createNode(NODE_RETURN_DECL, (yyvsp[-1].node), NULL, NULL, yylineno, NULL); }
#line 1469 "parser.tab.c"
    break;

  case 38: /* expr: var ATRIB expr  */
#line 197 "parser.y"
                          { (yyval.node) = createNode(NODE_EXPR, (yyvsp[-2].node), (yyvsp[0].node), "=", yylineno, NULL); }
#line 1475 "parser.tab.c"
    break;

  case 39: /* expr: simp_expr  */
#line 198 "parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1481 "parser.tab.c"
    break;

  case 40: /* var: ID  */
#line 202 "parser.y"
                          { 
        (yyval.node) = createNode(NODE_VAR, NULL, NULL, (yyvsp[0].sval), yylineno, NULL); 
        (yyval.node)->isArray = 0;
    }
#line 1490 "parser.tab.c"
    break;

  case 41: /* var: ID OBRACKT expr CBRACKT  */
#line 206 "parser.y"
                              {
        ASTNode* idNode = createNode(NODE_VAR, NULL, NULL, (yyvsp[-3].sval), yylineno, NULL);
        idNode->isArray = 1;  
        (yyval.node) = createNode(NODE_ARRAY_ACCESS, idNode, (yyvsp[-1].node), (yyvsp[-3].sval), yylineno, NULL);
        (yyval.node)->isArray = 1;  
    }
#line 1501 "parser.tab.c"
    break;

  case 42: /* simp_expr: sum_expr relational sum_expr  */
#line 215 "parser.y"
                                 {
        (yyval.node) = createNode(NODE_SIMP_EXPR, (yyvsp[-2].node), createNode(NODE_RELATIONAL, (yyvsp[-1].node), (yyvsp[0].node), NULL, yylineno, NULL), NULL, yylineno, NULL);
    }
#line 1509 "parser.tab.c"
    break;

  case 43: /* simp_expr: sum_expr  */
#line 218 "parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1515 "parser.tab.c"
    break;

  case 44: /* relational: NQ  */
#line 222 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, "!=", yylineno, NULL); }
#line 1521 "parser.tab.c"
    break;

  case 45: /* relational: BT  */
#line 223 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, ">", yylineno, NULL); }
#line 1527 "parser.tab.c"
    break;

  case 46: /* relational: BTE  */
#line 224 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, ">=", yylineno, NULL); }
#line 1533 "parser.tab.c"
    break;

  case 47: /* relational: LT  */
#line 225 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, "<", yylineno, NULL); }
#line 1539 "parser.tab.c"
    break;

  case 48: /* relational: LTE  */
#line 226 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, "<=", yylineno, NULL); }
#line 1545 "parser.tab.c"
    break;

  case 49: /* relational: EQ  */
#line 227 "parser.y"
                          { (yyval.node) = createNode(NODE_RELATIONAL, NULL, NULL, "==", yylineno, NULL); }
#line 1551 "parser.tab.c"
    break;

  case 50: /* sum_expr: sum_expr sum term  */
#line 231 "parser.y"
                          { (yyval.node) = createNode(NODE_SUM_EXPR, (yyvsp[-2].node), (yyvsp[0].node), (yyvsp[-1].node)->value, yylineno, NULL); }
#line 1557 "parser.tab.c"
    break;

  case 51: /* sum_expr: term  */
#line 232 "parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 1563 "parser.tab.c"
    break;

  case 52: /* sum: SUM  */
#line 236 "parser.y"
                         { (yyval.node) = createNode(NODE_SUM_EXPR, NULL, NULL, "+", yylineno, NULL); }
#line 1569 "parser.tab.c"
    break;

  case 53: /* sum: SUB  */
#line 237 "parser.y"
                        { (yyval.node) = createNode(NODE_SUM_EXPR, NULL, NULL, "-", yylineno, NULL); }
#line 1575 "parser.tab.c"
    break;

  case 54: /* term: term mult factor  */
#line 241 "parser.y"
                         { (yyval.node) = createNode(NODE_TERM, (yyvsp[-2].node), (yyvsp[0].node), (yyvsp[-1].node)->value, yylineno, NULL); }
#line 1581 "parser.tab.c"
    break;

  case 55: /* term: factor  */
#line 242 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1587 "parser.tab.c"
    break;

  case 56: /* mult: MULT  */
#line 246 "parser.y"
                        { (yyval.node) = createNode(NODE_MULT, NULL, NULL, "*", yylineno, NULL); }
#line 1593 "parser.tab.c"
    break;

  case 57: /* mult: DIV  */
#line 247 "parser.y"
                       { (yyval.node) = createNode(NODE_MULT, NULL, NULL, "/", yylineno, NULL); }
#line 1599 "parser.tab.c"
    break;

  case 58: /* factor: OPAREN expr CPAREN  */
#line 251 "parser.y"
                        { (yyval.node) = (yyvsp[-1].node); }
#line 1605 "parser.tab.c"
    break;

  case 59: /* factor: var  */
#line 252 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1611 "parser.tab.c"
    break;

  case 60: /* factor: activation  */
#line 253 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1617 "parser.tab.c"
    break;

  case 61: /* factor: NUM  */
#line 254 "parser.y"
                      {
        char numStr[32];
        sprintf(numStr, "%d", (yyvsp[0].ival));
        (yyval.node) = createNode(NODE_FACTOR, NULL, NULL, numStr, yylineno, NULL);
    }
#line 1627 "parser.tab.c"
    break;

  case 62: /* activation: ID OPAREN args CPAREN  */
#line 262 "parser.y"
                          {
        // Modificar para criar um nó de tipo NODE_FUNC para o identificador de função em vez de NODE_VAR
        ASTNode* idNode = createNode(NODE_FUNC, NULL, NULL, (yyvsp[-3].sval), yylineno, NULL);
        (yyval.node) = createNode(NODE_ACTIVATION, idNode, (yyvsp[-1].node), NULL, yylineno, NULL);
    }
#line 1637 "parser.tab.c"
    break;

  case 63: /* args: arg_list  */
#line 270 "parser.y"
                       { (yyval.node) = createNode(NODE_ARGS, (yyvsp[0].node), NULL, NULL, yylineno, NULL); }
#line 1643 "parser.tab.c"
    break;

  case 64: /* args: %empty  */
#line 271 "parser.y"
                       { (yyval.node) = NULL; }
#line 1649 "parser.tab.c"
    break;

  case 65: /* arg_list: arg_list COMMA expr  */
#line 275 "parser.y"
                        { (yyval.node) = createNode(NODE_ARG_LIST, (yyvsp[-2].node), (yyvsp[0].node), NULL, yylineno, NULL); }
#line 1655 "parser.tab.c"
    break;

  case 66: /* arg_list: expr  */
#line 276 "parser.y"
                       { (yyval.node) = createNode(NODE_ARG_LIST, (yyvsp[0].node), NULL, NULL, yylineno, NULL); }
#line 1661 "parser.tab.c"
    break;


#line 1665 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 279 "parser.y"


void yyerror(const char *s) {
    if(strcmp(s, "syntax error") != 0){
        printError("Erro sintático na linha %d: %s\n", yylineno, s);
        syntaxErrorCount++;
        //exit(2);
    }
}
