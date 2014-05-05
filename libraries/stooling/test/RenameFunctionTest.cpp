/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/

#include <string>

#include "Test.h"

using namespace testing;

class RenameFunctionTest : public Test
{
protected:
  RenameFunctionTest() {}
};

TEST_F(RenameFunctionTest, RenameEmptyFunctionDeclaration)
{
  const char* input = "\
void foo();\n\
void baz();\n\
";
  stooling::SourceCode s(input);

  s.renameFunction("foo", "bar");

  const char* expectedOutput = "\
void bar();\n\
void baz();\n\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(RenameFunctionTest, RenameFunctionWithArgsDeclaration)
{
  const char* input = "\
void foo(char c, int x);\n\
void baz();\n\
";
  stooling::SourceCode s(input);

  s.renameFunction("foo", "bar");

  const char* expectedOutput = "\
void bar(char c, int x);\n\
void baz();\n\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(RenameFunctionTest, RenameEmptyFunctionUsage)
{
  const char* input = "\
void foo();\n\
\n\
void baz() {\n\
  foo();\n\
}\
";
  stooling::SourceCode s(input);

  s.renameFunction("foo", "bar");

  const char* expectedOutput = "\
void bar();\n\
\n\
void baz() {\n\
  bar();\n\
}\
";
  ASSERT_EQ(expectedOutput, s.code());
}

TEST_F(RenameFunctionTest, RenameFunctionWithArgumentsUsage)
{
  const char* input = "\
void foo(int x, float y);\n\
\n\
void baz() {\n\
  foo(5, 4.0f);\n\
}\
";
  stooling::SourceCode s(input);

  s.renameFunction("foo", "bar");

  const char* expectedOutput = "\
void bar(int x, float y);\n\
\n\
void baz() {\n\
  bar(5, 4.0f);\n\
}\
";
  ASSERT_EQ(expectedOutput, s.code());
}

