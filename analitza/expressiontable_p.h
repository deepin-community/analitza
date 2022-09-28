// This file was generated by qlalr - DO NOT EDIT!
#ifndef EXPRESSIONTABLE_P_H
#define EXPRESSIONTABLE_P_H

class ExpressionTable
{
public:
  enum VariousConstants {
    EOF_SYMBOL = 0,
    otherwise_prec = 31,
    tAdd = 1,
    tAssig = 4,
    tAt = 28,
    tColon = 27,
    tComa = 12,
    tComment = 29,
    tDiv = 6,
    tEq = 21,
    tGeq = 25,
    tGt = 23,
    tId = 9,
    tLambda = 10,
    tLcb = 15,
    tLeq = 24,
    tLimits = 5,
    tLpr = 13,
    tLsp = 17,
    tLt = 22,
    tMul = 3,
    tNeq = 26,
    tPipe = 19,
    tPow = 7,
    tQm = 11,
    tRcb = 16,
    tRpr = 14,
    tRsp = 18,
    tString = 30,
    tSub = 2,
    tUniPow = 8,
    tVal = 20,
    uminus_prec = 32,

    ACCEPT_STATE = 106,
    RULE_COUNT = 60,
    STATE_COUNT = 109,
    TERMINAL_COUNT = 33,
    NON_TERMINAL_COUNT = 23,

    GOTO_INDEX_OFFSET = 109,
    GOTO_INFO_OFFSET = 400,
    GOTO_CHECK_OFFSET = 400
  };

  static const char  *const    spell [];
  static const short             lhs [];
  static const short             rhs [];

#ifndef QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO
  static const int     rule_index [];
  static const int      rule_info [];
#endif // QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO

  static const short    goto_default [];
  static const short  action_default [];
  static const short    action_index [];
  static const short     action_info [];
  static const short    action_check [];

  static inline int nt_action (int state, int nt)
  {
    const int yyn = action_index [GOTO_INDEX_OFFSET + state] + nt;
    if (yyn < 0 || action_check [GOTO_CHECK_OFFSET + yyn] != nt)
      return goto_default [nt];

    return action_info [GOTO_INFO_OFFSET + yyn];
  }

  static inline int t_action (int state, int token)
  {
    const int yyn = action_index [state] + token;

    if (yyn < 0 || action_check [yyn] != token)
      return - action_default [state];

    return action_info [yyn];
  }
};


#endif // EXPRESSIONTABLE_P_H

