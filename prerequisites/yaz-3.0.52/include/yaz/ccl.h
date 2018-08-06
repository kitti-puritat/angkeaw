/* This file is part of the YAZ toolkit.
 * Copyright (C) 1995-2009 Index Data.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Index Data nor the names of its contributors
 *       may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file ccl.h
    \brief Header with public definitions for CCL.
*/

/*
 * CCL - header file
 *
 *
 * Old Europagate Log:
 *
 * Revision 1.10  1996/01/08  08:41:22  adam
 * Minor changes.
 *
 * Revision 1.9  1995/07/20  08:15:16  adam
 * Bug fix: Token value for comma and OR were the same!
 *
 * Revision 1.8  1995/07/11  12:28:34  adam
 * New function: ccl_token_simple (split into simple tokens) and
 *  ccl_token_del (delete tokens).
 *
 * Revision 1.7  1995/05/16  09:39:38  adam
 * LICENSE.
 *
 * Revision 1.6  1995/05/11  14:04:03  adam
 * Changes in the reading of qualifier(s). New function: ccl_qual_fitem.
 * New variable ccl_case_sensitive, which controls whether reserved
 * words and field names are case sensitive or not.
 *
 * Revision 1.5  1995/02/23  08:32:11  adam
 * Changed header.
 *
 * Revision 1.3  1995/02/16  13:20:10  adam
 * Spell fix.
 *
 * Revision 1.2  1995/02/15  17:43:08  adam
 * Minor changes to the ccl interface. Bug fix in iso2709 module.
 *
 * Revision 1.1  1995/02/14  19:55:21  adam
 * Header files ccl.h/cclp.h are gone! They have been merged an
 * moved to ../include/ccl.h.
 *
 */

#ifndef CCL_H
#define CCL_H

#include <yaz/yconfig.h>
#include <stdio.h>
#include <yaz/xmalloc.h>
#include <yaz/wrbuf.h>

YAZ_BEGIN_CDECL
    
#define CCL_ERR_OK                0
#define CCL_ERR_TERM_EXPECTED     1
#define CCL_ERR_RP_EXPECTED       2
#define CCL_ERR_SETNAME_EXPECTED  3
#define CCL_ERR_OP_EXPECTED       4
#define CCL_ERR_BAD_RP            5
#define CCL_ERR_UNKNOWN_QUAL      6
#define CCL_ERR_DOUBLE_QUAL       7
#define CCL_ERR_EQ_EXPECTED       8
#define CCL_ERR_BAD_RELATION      9
#define CCL_ERR_TRUNC_NOT_LEFT   10
#define CCL_ERR_TRUNC_NOT_BOTH   11
#define CCL_ERR_TRUNC_NOT_RIGHT  12
    
/** \brief attribute node (type, value) pair as used in RPN */
struct ccl_rpn_attr {
    /** \brief next attribute */
    struct ccl_rpn_attr *next;
    /** \brief attribute set */
    char *set;
    /** \brief attribute type, Bib-1: 1=use, 2=relation, 3=position, .. */
    int type;
    /** \brief attribute value type (numeric or string) */
    int kind;
#define CCL_RPN_ATTR_NUMERIC 1
#define CCL_RPN_ATTR_STRING 2
    union {
        /** \brief numeric attribute value */
        int numeric;
        /** \brief string attribute value */
        char *str;
    } value;
};

/** \brief node type or RPN tree generated by the CCL parser */
enum ccl_rpn_kind {
    CCL_RPN_AND,
    CCL_RPN_OR,
    CCL_RPN_NOT,
    CCL_RPN_TERM,
    CCL_RPN_SET,
    CCL_RPN_PROX
};

/** \brief RPN tree structure node */
struct ccl_rpn_node {
    /** \brief node type, one of CCL_RPN_AND, CCL_RPN_OR,.. */
    enum ccl_rpn_kind kind;
    union {
        /** \brief Boolean including proximity 0=left, 1=right, 2=prox parms */
        struct ccl_rpn_node *p[3];
        /** \brief Attributes + Term */
        struct {
            char *term;
            char *qual;
            struct ccl_rpn_attr *attr_list;
        } t;
        /** Result set */
        char *setname;
    } u;
};

/** \brief CCL bibset, AKA profile */
typedef struct ccl_qualifiers *CCL_bibset;

/** \brief CCL parser */
typedef struct ccl_parser *CCL_parser;
    
/**
   \brief parse CCL find string using CCL profile return RPN tree
   
   Parses a CCL Find command in a simple C string. Returns CCL parse
   tree node describing RPN if parsing is successful. If parsing is
   unsuccesful, NULL is returned and error and pos is set accordingly.
*/
YAZ_EXPORT
struct ccl_rpn_node *ccl_find_str(CCL_bibset bibset,
                                  const char *str, int *error, int *pos);


/**
   \brief parse CCL find string with parser and return RPN tree
   
   Parses a CCL Find command in a simple C string. Returns CCL parse
   tree node describing RPN if parsing is successful. If parsing is
   unsuccesful, NULL is returned and error and pos is set accordingly.
*/
YAZ_EXPORT
struct ccl_rpn_node *ccl_parser_find_str(CCL_parser cclp, const char *str);

/** Set case sensitivity for parser */
YAZ_EXPORT
void ccl_parser_set_case(CCL_parser p, int case_sensitivity_flag);

/** Return english-readable error message for CCL parser error number */
YAZ_EXPORT
const char *ccl_err_msg(int ccl_errno);

/** Delete RPN tree returned by ccl_find */
YAZ_EXPORT
void ccl_rpn_delete(struct ccl_rpn_node *rpn);

/** Dump RPN tree in readable format to fd_out */
YAZ_EXPORT
void ccl_pr_tree(struct ccl_rpn_node *rpn, FILE *fd_out);

/** Add qualifier and supply attribute pairs for it */
YAZ_EXPORT
void ccl_qual_add(CCL_bibset b, const char *name, int no, int *attr);

/** Add qualifier and supply attributes pairs+attribute set for it */
YAZ_EXPORT
void ccl_qual_add_set(CCL_bibset b, const char *name, int no,
                      int *type, int *value, char **svalue, char **attsets);

/** Add special qualifier */
YAZ_EXPORT
void ccl_qual_add_special(CCL_bibset bibset, const char *n, const char *cp);

/** Add combo qualifier */
YAZ_EXPORT
void ccl_qual_add_combi(CCL_bibset b, const char *n, const char **names);

/** Read CCL qualifier list spec from file inf */
YAZ_EXPORT
void ccl_qual_file(CCL_bibset bibset, FILE *inf);

/** Read CCL qualifier list spec from file inf */
YAZ_EXPORT
int ccl_qual_fname(CCL_bibset bibset, const char *fname);

/** Add CCL qualifier as buf spec(multiple lines). */
YAZ_EXPORT
void ccl_qual_buf(CCL_bibset bibset, const char *buf);

/** Add CCL qualifier as line spec. Note: line is _modified_ */
YAZ_EXPORT
void ccl_qual_line(CCL_bibset bibset, char *line);

/* Add CCL qualifier by using qual_name + value pair */
YAZ_EXPORT
void ccl_qual_fitem(CCL_bibset bibset, const char *value,
                    const char *qual_name);

/** Make CCL qualifier set */
YAZ_EXPORT
CCL_bibset ccl_qual_mk(void);

/** Delete CCL qualifier set */
YAZ_EXPORT
void ccl_qual_rm(CCL_bibset *b);

/** Char-to-upper function */
extern int(*ccl_toupper)(int c);

/** CCL version of ccl_stricmp */
YAZ_EXPORT
int ccl_stricmp(const char *s1, const char *s2);

/** CCL version of ccl_memicmp */
YAZ_EXPORT
int ccl_memicmp(const char *s1, const char *s2, size_t n);

/** Create CCL parser */
YAZ_EXPORT
CCL_parser ccl_parser_create(CCL_bibset bibset);

/** Destroy CCL parser */
YAZ_EXPORT
void ccl_parser_destroy(CCL_parser p);

/** Search for special qualifier */
YAZ_EXPORT
const char **ccl_qual_search_special(CCL_bibset b, const char *name);
/** Pretty-print CCL RPN node tree to WRBUF */
YAZ_EXPORT
void ccl_pquery(WRBUF w, struct ccl_rpn_node *p);

YAZ_EXPORT
int ccl_parser_get_error(CCL_parser cclp, int *pos);

YAZ_EXPORT
struct ccl_rpn_node *ccl_rpn_node_create(enum ccl_rpn_kind kind);

YAZ_EXPORT
void ccl_add_attr_numeric(struct ccl_rpn_node *p, const char *set,
                          int type, int value);

YAZ_EXPORT
void ccl_add_attr_string(struct ccl_rpn_node *p, const char *set,
                         int type, char *value);

YAZ_EXPORT
int ccl_search_stop(CCL_bibset bibset, const char *qname,
                    const char *src_str, size_t src_len);


/** \brief stop words handle (pimpl) */
typedef struct ccl_stop_words *ccl_stop_words_t;

/** \brief creates stop words handle */
YAZ_EXPORT
ccl_stop_words_t ccl_stop_words_create(void);

/** \brief destroys stop words handle */
YAZ_EXPORT
void ccl_stop_words_destroy(ccl_stop_words_t csw);

/** \brief removes stop words from RPN tree */
YAZ_EXPORT
int ccl_stop_words_tree(ccl_stop_words_t csw,
                        CCL_bibset bibset, struct ccl_rpn_node **t);

/** \brief returns information about removed "stop" words */
YAZ_EXPORT
int ccl_stop_words_info(ccl_stop_words_t csw, int idx,
                        const char **qualname, const char **term);

#ifndef ccl_assert
#define ccl_assert(x) ;
#endif


/** \brief common attributes

   use (1)

   relation (2)
                            -1  none
                             0  ordered
                           1-6  relation (<, <=, =, >=, >, <>)

   position (3)
                            -1  none
                             1  first in field
                             2  first in sub field
                             3  any position in field
   structure (4)
                            -1  none
                             0  word/phrase auto select
                             1  phrase
                             2  word
                             3  key
                             4  year
                             5  date (normalized)
                             6  word list 
                           100  date (un-normalized)
                           101  name (normalized)
                           102  name (un-normalized)
   truncation (5)                            
   completeness (6)
*/

#define CCL_BIB1_USE 1
#define CCL_BIB1_REL 2
#define CCL_BIB1_POS 3
#define CCL_BIB1_STR 4
#define CCL_BIB1_TRU 5
#define CCL_BIB1_COM 6

#define CCL_BIB1_STR_WP (-1)
#define CCL_BIB1_STR_AND_LIST (-2)
#define CCL_BIB1_STR_OR_LIST (-3)
#define CCL_BIB1_REL_ORDER (-1)
#define CCL_BIB1_REL_PORDER (-2)

#define CCL_BIB1_TRU_CAN_LEFT (-1)
#define CCL_BIB1_TRU_CAN_RIGHT (-2)
#define CCL_BIB1_TRU_CAN_BOTH  (-3)
#define CCL_BIB1_TRU_CAN_NONE  (-4)



YAZ_END_CDECL

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

