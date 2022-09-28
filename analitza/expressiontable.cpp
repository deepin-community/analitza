// This file was generated by qlalr - DO NOT EDIT!
#include "expressiontable_p.h"

const char *const ExpressionTable::spell [] = {
  "end of file", "+", "-", "*", ":=", "..", "/", "^", "²", "identifier", 
  "->", "?", ",", "(", ")", "{", "}", "[", "]", "|", 
  "value", "=", "<", ">", ">=", "<=", "!=", ":", "@", "//Comment//", 
  "abc", nullptr, nullptr, 
#ifndef QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO
"Program", "Declaration", "Expression", "Id", "String", "Number", "UniPow", 
  "Value", "PrimaryExpression", "BlockExpression", "SubscriptExpression", "PrimaryExpressionExt", "FunctionCall", "FunctionId", "FBody", "PipedCall", "LambdaExpression", 
  "Parameters", "BVars", "Limits", "BValue", "BVarList", "$accept"
#endif // QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO
};

const short ExpressionTable::lhs [] = {
  33, 33, 33, 34, 36, 37, 38, 39, 40, 40, 
  40, 41, 41, 43, 44, 44, 44, 44, 35, 46, 
  46, 35, 45, 45, 48, 48, 35, 35, 47, 47, 
  47, 47, 42, 42, 49, 35, 35, 35, 35, 35, 
  35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 
  35, 50, 50, 51, 51, 53, 54, 54, 52, 55};

const short ExpressionTable::rhs [] = {
  0, 1, 1, 3, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 4, 3, 1, 1, 1, 1, 3, 
  1, 2, 4, 3, 3, 3, 1, 1, 1, 3, 
  5, 5, 3, 4, 3, 2, 2, 3, 3, 3, 
  3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 
  3, 1, 3, 1, 3, 1, 3, 3, 3, 2};


#ifndef QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO
const int ExpressionTable::rule_info [] = {
    33
  , 33, 34
  , 33, 35
  , 34, 36, 4, 35
  , 36, 9
  , 37, 30
  , 38, 20
  , 39, 8
  , 40, 38
  , 40, 37
  , 40, 36
  , 41, 40
  , 41, 42
  , 43, 44, 17, 35, 18
  , 44, 13, 35, 14
  , 44, 41
  , 44, 43
  , 44, 45
  , 35, 44
  , 46, 13, 35, 14
  , 46, 36
  , 35, 46, 41
  , 45, 46, 13, 47, 14
  , 45, 46, 13, 14
  , 48, 35, 19, 46
  , 48, 35, 19, 49
  , 35, 48
  , 35, 49
  , 47, 50
  , 47, 50, 27, 51
  , 47, 50, 27, 51, 21, 52
  , 47, 50, 27, 51, 28, 35
  , 42, 36, 15, 16
  , 42, 36, 15, 50, 16
  , 49, 51, 10, 35
  , 35, 2, 35
  , 35, 11, 35
  , 35, 35, 1, 35
  , 35, 35, 2, 35
  , 35, 35, 3, 35
  , 35, 35, 6, 35
  , 35, 35, 7, 35
  , 35, 35, 21, 35
  , 35, 35, 24, 35
  , 35, 35, 25, 35
  , 35, 35, 22, 35
  , 35, 35, 23, 35
  , 35, 35, 26, 35
  , 35, 38, 44
  , 35, 35, 39
  , 35, 35, 11, 35
  , 50, 35
  , 50, 50, 12, 35
  , 51, 53
  , 51, 13, 54, 14
  , 53, 36
  , 54, 53, 12, 53
  , 54, 54, 12, 53
  , 52, 44, 5, 44
  , 55, 33, 0};

const int ExpressionTable::rule_index [] = {
  0, 1, 3, 5, 9, 11, 13, 15, 17, 19, 
  21, 23, 25, 27, 32, 36, 38, 40, 42, 44, 
  48, 50, 53, 58, 62, 66, 70, 72, 74, 76, 
  80, 86, 92, 96, 101, 105, 108, 111, 115, 119, 
  123, 127, 131, 135, 139, 143, 147, 151, 155, 158, 
  161, 165, 167, 171, 173, 177, 179, 183, 187, 191};
#endif // QLALR_NO_EXPRESSIONTABLE_DEBUG_INFO

const short ExpressionTable::action_default [] = {
  1, 54, 0, 13, 2, 3, 18, 0, 11, 28, 
  9, 27, 16, 19, 0, 10, 17, 12, 5, 0, 
  0, 6, 0, 7, 0, 35, 11, 50, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 8, 38, 41, 43, 45, 47, 44, 46, 40, 
  48, 25, 21, 26, 0, 54, 0, 0, 11, 0, 
  57, 56, 0, 55, 58, 20, 0, 52, 0, 33, 
  0, 34, 53, 42, 51, 39, 11, 9, 22, 0, 
  0, 29, 24, 23, 0, 30, 0, 0, 0, 32, 
  0, 11, 31, 0, 0, 0, 0, 59, 0, 14, 
  0, 15, 0, 0, 4, 49, 60, 37, 36};

const short ExpressionTable::goto_default [] = {
  14, 4, 67, 26, 15, 10, 27, 17, 12, 3, 
  16, 13, 6, 7, 80, 11, 9, 68, 2, 92, 
  1, 56, 0};

const short ExpressionTable::action_index [] = {
  120, -33, -4, -33, -33, 271, -33, 33, 128, -33, 
  59, -33, -33, -9, 35, -33, -33, -33, -33, 120, 
  120, -33, 120, -33, 120, 271, 73, -33, 120, 64, 
  120, 120, 120, 120, 120, 120, 120, 4, 120, 120, 
  120, -33, 52, 31, 176, 176, 176, 176, 176, 31, 
  117, -33, -1, -33, 120, 0, 6, 367, 144, -5, 
  -33, -33, -6, -33, -33, -33, 96, 271, 3, -33, 
  120, -33, 271, 13, 239, 89, -10, -33, -33, 207, 
  2, 10, -33, -33, 14, 5, -2, 120, 32, 271, 
  -3, 15, -33, 19, 120, 59, 120, -15, 303, -33, 
  335, 34, -11, 120, 271, -17, -33, 271, 31, 

  40, -23, -23, -23, -23, -23, -23, 60, -23, -23, 
  43, -23, -23, -23, -23, -23, -23, -23, -23, 27, 
  17, -23, 1, -23, 15, -23, -23, -23, 14, 13, 
  11, 2, 10, 9, 8, 7, 5, 22, 3, 4, 
  6, -23, -23, -23, -23, -23, -23, -23, -23, -23, 
  -23, -23, -23, -23, 29, -23, -23, -23, -23, 25, 
  -23, -23, 24, -23, -23, -23, -23, -23, -23, -23, 
  12, -23, -23, -23, -23, -23, -23, -23, -23, 16, 
  -23, -23, -23, -23, 21, -23, 20, -2, 48, -23, 
  -23, -23, -23, -23, -1, 47, 0, -23, -23, -23, 
  -23, -23, -23, 19, -23, -23, -23, -23, -23};

const short ExpressionTable::action_info [] = {
  96, 59, 96, 18, 18, 66, 24, 18, 96, -56, 
  79, 0, 59, 18, 0, 70, 83, 54, 62, 71, 
  63, 41, 70, 18, 95, 0, 88, 86, -21, 0, 
  66, 0, 37, 87, 0, 106, 96, 84, 38, 41, 
  0, 18, 18, -20, 0, 94, 79, -20, 0, 0, 
  37, 0, 23, 23, -20, 35, 0, 0, 29, 38, 
  41, 0, 21, 21, -20, 0, 22, 0, 18, 0, 
  0, 37, 94, 18, 0, 20, 0, 19, 0, 23, 
  0, 0, -21, -56, 23, 0, -21, 0, 66, 21, 
  0, 0, 35, -21, 21, 29, 38, 41, 22, 0, 
  0, 0, 0, -21, 0, 18, 0, 20, 37, 19, 
  0, 0, 69, 0, 0, 0, 23, 0, 28, 40, 
  35, 0, 22, 29, 38, 41, 21, 0, 0, 18, 
  0, 20, 103, 19, 0, 0, 37, -21, -56, 0, 
  23, -21, 0, 66, 0, 0, 0, 0, -21, 0, 
  21, 0, 0, -21, -56, 0, -56, -21, -21, 66, 
  0, 0, 0, 0, -21, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, -21, 0, 0, 28, 40, 35, 
  0, 0, 29, 38, 41, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 
  0, 0, 0, 0, 0, 0, 18, 0, 20, 0, 
  19, 82, 0, 0, 0, 0, 0, 23, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 
  28, 40, 35, 0, 0, 29, 38, 41, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 37, 0, 
  30, 34, 32, 33, 31, 36, 0, 0, 0, 0, 
  0, 0, 28, 40, 35, 0, 0, 29, 38, 41, 
  0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 
  37, 0, 30, 34, 32, 33, 31, 36, 0, 0, 
  0, 0, 0, 0, 28, 40, 35, 0, 0, 29, 
  38, 41, 0, 0, 39, 0, 0, 0, 0, 0, 
  0, 99, 37, 0, 30, 34, 32, 33, 31, 36, 
  0, 0, 0, 0, 0, 0, 28, 40, 35, 0, 
  0, 29, 38, 41, 0, 0, 39, 0, 0, 101, 
  0, 0, 0, 0, 37, 0, 30, 34, 32, 33, 
  31, 36, 0, 0, 0, 0, 0, 0, 28, 40, 
  35, 0, 0, 29, 38, 41, 0, 0, 39, 0, 
  0, 65, 0, 0, 0, 0, 37, 0, 30, 34, 
  32, 33, 31, 36, 0, 0, 0, 0, 0, 0, 

  89, 100, 98, 108, 45, 73, 74, 50, 75, 49, 
  48, 47, 46, 44, 72, 43, 42, 25, 0, 107, 
  0, 104, 0, 61, 61, 52, 0, 61, 61, 100, 
  58, 57, 58, 81, 0, 51, 0, 0, 53, 85, 
  102, 0, 5, 8, 64, 60, 91, 55, 77, 55, 
  91, 91, 77, 77, 105, 0, 90, 0, 97, 93, 
  90, 90, 0, 76, 0, 77, 0, 0, 78, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0};

const short ExpressionTable::action_check [] = {
  17, 12, 17, 9, 9, 15, 10, 9, 17, 10, 
  13, -1, 12, 9, -1, 12, 14, 13, 12, 16, 
  14, 8, 12, 9, 5, -1, 21, 13, 13, -1, 
  15, -1, 19, 28, -1, 0, 17, 27, 7, 8, 
  -1, 9, 9, 9, -1, 13, 13, 13, -1, -1, 
  19, -1, 20, 20, 20, 3, -1, -1, 6, 7, 
  8, -1, 30, 30, 30, -1, 2, -1, 9, -1, 
  -1, 19, 13, 9, -1, 11, -1, 13, -1, 20, 
  -1, -1, 9, 10, 20, -1, 13, -1, 15, 30, 
  -1, -1, 3, 20, 30, 6, 7, 8, 2, -1, 
  -1, -1, -1, 30, -1, 9, -1, 11, 19, 13, 
  -1, -1, 16, -1, -1, -1, 20, -1, 1, 2, 
  3, -1, 2, 6, 7, 8, 30, -1, -1, 9, 
  -1, 11, 4, 13, -1, -1, 19, 9, 10, -1, 
  20, 13, -1, 15, -1, -1, -1, -1, 20, -1, 
  30, -1, -1, 9, 10, -1, 12, 13, 30, 15, 
  -1, -1, -1, -1, 20, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, 30, -1, -1, 1, 2, 3, 
  -1, -1, 6, 7, 8, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, 19, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 
  -1, -1, -1, -1, -1, -1, 9, -1, 11, -1, 
  13, 14, -1, -1, -1, -1, -1, 20, -1, -1, 
  -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, 
  1, 2, 3, -1, -1, 6, 7, 8, -1, -1, 
  -1, -1, -1, -1, -1, -1, -1, -1, 19, -1, 
  21, 22, 23, 24, 25, 26, -1, -1, -1, -1, 
  -1, -1, 1, 2, 3, -1, -1, 6, 7, 8, 
  -1, -1, 11, -1, -1, -1, -1, -1, -1, -1, 
  19, -1, 21, 22, 23, 24, 25, 26, -1, -1, 
  -1, -1, -1, -1, 1, 2, 3, -1, -1, 6, 
  7, 8, -1, -1, 11, -1, -1, -1, -1, -1, 
  -1, 18, 19, -1, 21, 22, 23, 24, 25, 26, 
  -1, -1, -1, -1, -1, -1, 1, 2, 3, -1, 
  -1, 6, 7, 8, -1, -1, 11, -1, -1, 14, 
  -1, -1, -1, -1, 19, -1, 21, 22, 23, 24, 
  25, 26, -1, -1, -1, -1, -1, -1, 1, 2, 
  3, -1, -1, 6, 7, 8, -1, -1, 11, -1, 
  -1, 14, -1, -1, -1, -1, 19, -1, 21, 22, 
  23, 24, 25, 26, -1, -1, -1, -1, -1, -1, 

  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
  2, 2, 2, 2, 2, 2, 2, 2, -1, 2, 
  -1, 2, -1, 3, 3, 3, -1, 3, 3, 2, 
  3, 2, 3, 17, -1, 13, -1, -1, 16, 18, 
  20, -1, 2, 3, 20, 20, 3, 20, 5, 20, 
  3, 3, 5, 5, 11, -1, 13, -1, 11, 11, 
  13, 13, -1, 3, -1, 5, -1, -1, 8, -1, 
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
  -1, -1, -1};
