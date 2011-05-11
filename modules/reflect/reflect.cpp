/*
    Copyright (c) 2011 Sencha Inc.
    Copyright (c) 2010 Sencha Inc.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <v8.h>

#include <JSGlobalData.h>
#include <SourceCode.h>
#include <SyntaxTree.h>
#include <UString.h>

using namespace v8;

static const char* operatorAsText(JSC::SyntaxTree::Node::OperatorType op)
{
    switch (op) {
    case JSC::SyntaxTree::Node::TypeofOperator: return "typeof"; break;
    case JSC::SyntaxTree::Node::DeleteOperator: return "delete"; break;
    case JSC::SyntaxTree::Node::LogicalNotOperator: return "!"; break;
    case JSC::SyntaxTree::Node::LogicalOrOperator: return "||"; break;
    case JSC::SyntaxTree::Node::LogicalAndOperator: return "&&"; break;
    case JSC::SyntaxTree::Node::BitwiseNotOperator: return "!"; break;
    case JSC::SyntaxTree::Node::BitwiseOrOperator: return "|"; break;
    case JSC::SyntaxTree::Node::BitwiseXorOperator: return "^"; break;
    case JSC::SyntaxTree::Node::BitwiseAndOperator: return "&"; break;
    case JSC::SyntaxTree::Node::EqualOperator: return "=="; break;
    case JSC::SyntaxTree::Node::NotEqualOperator: return "!="; break;
    case JSC::SyntaxTree::Node::StringEqualOperator: return "==="; break;
    case JSC::SyntaxTree::Node::StringNotEqualOperator: return "!=="; break;
    case JSC::SyntaxTree::Node::LessThanOperator: return "<"; break;
    case JSC::SyntaxTree::Node::GreaterThanOperator: return ">"; break;
    case JSC::SyntaxTree::Node::LessThanOrEqualOperator: return ">="; break;
    case JSC::SyntaxTree::Node::GreaterThanOrEqualOperator: return "<="; break;
    case JSC::SyntaxTree::Node::InstanceofOperator: return "instance of";
    case JSC::SyntaxTree::Node::IntokenOperator: return "in";
    case JSC::SyntaxTree::Node::LeftShiftOperator: return "<<";
    case JSC::SyntaxTree::Node::RightShiftOperator: return ">>";
    case JSC::SyntaxTree::Node::ZeroFillRightShiftOperator: return ">>>";
    case JSC::SyntaxTree::Node::AddOperator: return "+";
    case JSC::SyntaxTree::Node::SubtractOperator: return "-";
    case JSC::SyntaxTree::Node::MultiplyOperator: return "*";
    case JSC::SyntaxTree::Node::DivideOperator: return "/";
    case JSC::SyntaxTree::Node::ModulusOperator: return "%";
    case JSC::SyntaxTree::Node::PlusPlusOperator: return "++";
    case JSC::SyntaxTree::Node::MinusMinusOperator: return "--";
    case JSC::SyntaxTree::Node::AssignEqual: return "=";
    case JSC::SyntaxTree::Node::AssignAdd: return "+=";
    case JSC::SyntaxTree::Node::AssignSubtract: return "-=";
    case JSC::SyntaxTree::Node::AssignMultiply: return "*=";
    case JSC::SyntaxTree::Node::AssignDivide: return "/=";
    case JSC::SyntaxTree::Node::AssignModulus: return "%=";
    case JSC::SyntaxTree::Node::AssignLeftShift: return ">>=";
    case JSC::SyntaxTree::Node::AssignRightShift: return "<<=";
    case JSC::SyntaxTree::Node::AssignZeroFillRightShift: return ">>>=";
    case JSC::SyntaxTree::Node::AssignAnd: return "&=";
    case JSC::SyntaxTree::Node::AssignXor: return "^=";
    case JSC::SyntaxTree::Node::AssignOr: return "|=";
    default: CRASH(); break;
    }
    return 0;
}

Handle<Value> convertNode(JSC::SyntaxTree::Node*, int);

Handle<Array> convertChildren(JSC::SyntaxTree::Node* n, int indent)
{
    Handle<Array> children = Array::New(n->childCount());
    for (int index = 0; index < n->childCount(); ++index)
        children->Set(index, convertNode(n->childAt(index), indent + 1));
    return children;
}

Handle<Value> convertChild(JSC::SyntaxTree::Node* n, int index, int indent)
{
    if (n)
        if (index >= 0 && index < n->childCount())
            return convertNode(n->childAt(index), indent);

    return Null();
}

Handle<String> convertUString(const JSC::UString& ustring)
{
    return String::New(ustring.characters(), ustring.length());
}

Handle<Value> convertNode(JSC::SyntaxTree::Node* n, int indent)
{
    if (!n)
        return Null();

    if (n->type() == JSC::SyntaxTree::Node::ArgumentsType) {
        if (n->childCount() && n->childAt(0))
            return convertNode(n->childAt(0), indent);
        else
            return Array::New();
    }

    if (n->type() == JSC::SyntaxTree::Node::ArgumentsListType) {
        return convertChildren(n, indent + 1);
    }

    if (n->type() == JSC::SyntaxTree::Node::ArrayType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ArrayExpression"));
        if (n->childCount() > 0)
            object->Set(String::New("elements"), convertChildren(n->childAt(0), indent + 1));
        else
            object->Set(String::New("elements"), Array::New());
        return object;
    }

    // FIXME: should be variable init inside declaration?
    if (n->type() == JSC::SyntaxTree::Node::AssignmentExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("AssignmentExpression"));
        object->Set(String::New("operator"), String::New(operatorAsText(n->op())));
        object->Set(String::New("left"), convertChild(n, 0, indent + 1));
        object->Set(String::New("right"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::BinaryExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("BinaryExpression"));
        object->Set(String::New("operator"), String::New(operatorAsText(n->op())));
        object->Set(String::New("left"), convertChild(n, 0, indent + 1));
        object->Set(String::New("right"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::BlockStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("BlockStatement"));
        if (n->childCount() == 1 && n->childAt(0) && n->childAt(0)->type() == JSC::SyntaxTree::Node::SourceElementsType)
            object->Set(String::New("body"), convertChildren(n->childAt(0), indent + 1));
        else
            object->Set(String::New("body"), Array::New());
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::BooleanExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Literal"));
        object->Set(String::New("objtype"), String::New("Boolean"));
        object->Set(String::New("value"), Boolean::New(n->boolean()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::BracketAccessType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("MemberExpression"));
        object->Set(String::New("accesstype"), String::New("Bracket"));
        object->Set(String::New("object"), convertChild(n, 0, indent + 1));
        object->Set(String::New("property"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::BreakStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("BreakStatement"));
        if (n->identifier().ustring().isEmpty())
            object->Set(String::New("label"), Null());
        else
            object->Set(String::New("label"), convertUString(n->identifier().ustring()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ClauseListType) {
        return n->childCount() ? convertChildren(n, indent + 1) : Array::New();
    }

    if (n->type() == JSC::SyntaxTree::Node::ClauseType) {
        Handle<Array> consequent = Array::New();
        if (n->childCount() && n->childAt(1))
            consequent->Set(0, convertNode(n->childAt(1), indent + 1));
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("SwitchCase"));
        object->Set(String::New("test"), convertChild(n, 0, indent + 1));
        object->Set(String::New("consequent"), consequent);
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::CommaType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("SequenceExpression"));
        object->Set(String::New("expressions"), convertChildren(n, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ContinueStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ContinueStatement"));
        if (n->identifier().ustring().isEmpty())
            object->Set(String::New("label"), Null());
        else
            object->Set(String::New("label"), convertUString(n->identifier().ustring()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ConditionalExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ConditionalExpression"));
        object->Set(String::New("test"), convertChild(n, 0, indent + 1));
        object->Set(String::New("consequent"), convertChild(n, 1, indent + 1));
        object->Set(String::New("alternate"), convertChild(n, 2, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::DotAccessType) {
        Handle<Object> property = Object::New();
        property->Set(String::New("type"), String::New("Identifier"));
        property->Set(String::New("name"), convertUString(n->identifier().ustring()));

        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("MemberExpression"));
        object->Set(String::New("accesstype"), String::New("Dot"));
        object->Set(String::New("object"), convertChild(n, 0, indent + 1));
        object->Set(String::New("property"), property);
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::DoWhileStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("DoWhileStatement"));
        object->Set(String::New("body"), convertChild(n, 0, indent + 1));
        object->Set(String::New("test"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ElementListType) {
        return convertChildren(n, indent + 1);
    }

    if (n->type() == JSC::SyntaxTree::Node::EmptyStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("EmptyStatement"));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ExpressionStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ExpressionStatement"));
        object->Set(String::New("expression"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ExpressionStatement"));
        object->Set(String::New("expression"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ForInLoopType) {
        if (n->childAt(0)) {
            Handle<Object> object = Object::New();
            object->Set(String::New("type"), String::New("ForInStatement"));
            object->Set(String::New("right"), convertChild(n, 1, indent + 1));
            object->Set(String::New("body"), convertChild(n, 2, indent + 1));
            object->Set(String::New("each"), Boolean::New(false));
            return object;
        } else {
            Handle<Object> id = Object::New();
            id->Set(String::New("type"), String::New("Identifier"));
            id->Set(String::New("name"), convertUString(n->identifier().ustring()));
            Handle<Object> declarator = Object::New();
            declarator->Set(String::New("type"), String::New("VariableDeclarator"));
            declarator->Set(String::New("id"), id);
            declarator->Set(String::New("init"), Null());

            Handle<Array> declarationArray = Array::New();
            declarationArray->Set(0, declarator);
            Handle<Object> declaration = Object::New();
            declaration->Set(String::New("type"), String::New("VariableDeclaration"));
            declaration->Set(String::New("declarations"), declarationArray);

            Handle<Object> object = Object::New();
            object->Set(String::New("type"), String::New("ForInStatement"));
            object->Set(String::New("left"), declaration);
            object->Set(String::New("right"), convertChild(n, 1, indent + 1));
            object->Set(String::New("body"), convertChild(n, 2, indent + 1));
            object->Set(String::New("each"), Boolean::New(false));
            return object;
        }
    }

    if (n->type() == JSC::SyntaxTree::Node::ForLoopType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ForStatement"));
        object->Set(String::New("init"), convertChild(n, 0, indent + 1));
        object->Set(String::New("test"), convertChild(n, 1, indent + 1));
        object->Set(String::New("update"), convertChild(n, 2, indent + 1));
        object->Set(String::New("body"), convertChild(n, 3, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::FormalParameterListType) {
        Handle<Array> params = Array::New();
        JSC::SyntaxTree::Node* pnode = n;
        int index = 0;
        while (pnode->type() == JSC::SyntaxTree::Node::FormalParameterListType) {
            Handle<Object> param = Object::New();
            param->Set(String::New("type"), String::New("Identifier"));
            param->Set(String::New("name"), convertUString(pnode->identifier().ustring()));
            params->Set(index++, param);
            // linked-list of parameter identifiers
            if (pnode->childCount() > 0)
                pnode = pnode->childAt(0);
            else
                break;
        }
        return params;
    }

    if (n->type() == JSC::SyntaxTree::Node::FunctionBodyType) {
        if (n->childCount() && n->childAt(0)) {
            return convertNode(n->childAt(0), indent);
        } else {
            Handle<Object> object = Object::New();
            object->Set(String::New("type"), String::New("BlockStatement"));
            object->Set(String::New("body"), Array::New());
            return object;
        }
    }

    if (n->type() == JSC::SyntaxTree::Node::FunctionCallType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("CallExpression"));
        object->Set(String::New("callee"), convertChild(n, 0, indent + 1));
        object->Set(String::New("arguments"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::FunctionDeclStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("FunctionExpression"));
        if (n->identifier().ustring().isEmpty())
            object->Set(String::New("id"), Null());
        else
            object->Set(String::New("id"), convertUString(n->identifier().ustring()));
        if (n->childCount() && n->childAt(0))
            object->Set(String::New("params"), convertChild(n, 0, indent + 1));
        else
            object->Set(String::New("params"), Array::New());
        object->Set(String::New("body"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::FunctionExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("FunctionExpression"));
        if (n->identifier().ustring().isEmpty())
            object->Set(String::New("id"), Null());
        else
            object->Set(String::New("id"), convertUString(n->identifier().ustring()));
        if (n->childCount() && n->childAt(0))
            object->Set(String::New("params"), convertChild(n, 0, indent + 1));
        else
            object->Set(String::New("params"), Array::New());
        object->Set(String::New("body"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::IdentifierExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Identifier"));
        object->Set(String::New("name"), convertUString(n->identifier().ustring()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::IfStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("IfStatement"));
        object->Set(String::New("test"), convertChild(n, 0, indent + 1));
        object->Set(String::New("consequent"), convertChild(n, 1, indent + 1));
        object->Set(String::New("alternate"), convertChild(n, 2, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::NewExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("NewExpression"));
        object->Set(String::New("callee"), convertChild(n, 0, indent + 1));
        object->Set(String::New("arguments"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::NullType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Literal"));
        object->Set(String::New("objtype"), String::New("Null"));
        object->Set(String::New("value"), Null());
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::NumberExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Literal"));
        object->Set(String::New("objtype"), String::New("Number"));
        object->Set(String::New("value"), Integer::New(n->number()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ObjectLiteralType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ObjectExpression"));
        if (n->childCount() && n->childAt(0) && n->childAt(0)->type() == JSC::SyntaxTree::Node::PropertyListType)
            object->Set(String::New("properties"), convertChildren(n->childAt(0), indent + 1));
        else
            object->Set(String::New("properties"), convertChildren(n, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::PostfixType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("UpdateExpression"));
        object->Set(String::New("operator"), String::New(operatorAsText(n->op())));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        object->Set(String::New("prefix"), Boolean::New(false));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::PrefixType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("UpdateExpression"));
        object->Set(String::New("operator"), String::New(operatorAsText(n->op())));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        object->Set(String::New("prefix"), Boolean::New(true));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::PropertyType) {
        Handle<Object> key = Object::New();
        key->Set(String::New("type"), String::New("Identifier"));
        key->Set(String::New("name"), convertUString(n->identifier().ustring()));
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Property"));
        object->Set(String::New("key"), key);
        object->Set(String::New("value"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::RegexType) {
        Handle<String> regex = convertUString(n->identifier().ustring());
        regex = String::Concat(String::New("/"), regex);
        regex = String::Concat(regex, String::New("/"));
        regex = String::Concat(regex, convertUString(n->string()));
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Literal"));
        object->Set(String::New("objtype"), String::New("RegEx"));
        object->Set(String::New("value"), regex);
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ReturnStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ReturnStatement"));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ResolveType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Identifier"));
        object->Set(String::New("name"), convertUString(n->identifier().ustring()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::SourceElementsType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Program"));
        if (indent > 0)
            object->Set(String::New("type"), String::New("BlockStatement"));
        object->Set(String::New("body"), convertChildren(n, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::StringExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("Literal"));
        object->Set(String::New("objtype"), String::New("String"));
        object->Set(String::New("value"), convertUString(n->string()));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::SwitchStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("SwitchStatement"));
        object->Set(String::New("discriminant"), convertChild(n, 0, indent + 1));
        if (n->childAt(1)) {
            // fold 'default' clause into another
            JSC::SyntaxTree::Node clauses(*n->childAt(1));
            if (n->childAt(2))
                clauses.append(n->childAt(2));
            object->Set(String::New("cases"), convertNode(&clauses, indent + 1));
        } else {
            if (n->childAt(2)) {
                object->Set(String::New("cases"), convertChildren(n->childAt(2), indent + 1));
            }
        }
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ThisType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ThisExpression"));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::ThrowStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("ThrowStatement"));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::TryStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("TryStatement"));
        object->Set(String::New("block"), convertChild(n, 0, indent + 1));
        object->Set(String::New("handler"), convertChild(n, 1, indent + 1));
        object->Set(String::New("finalizer"), convertChild(n, 2, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::UnaryExpressionType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("UnaryExpression"));
        object->Set(String::New("operator"), String::New(operatorAsText(n->op())));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::VariableDeclarationType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("VariableDeclaration"));
        object->Set(String::New("declarations"), convertChildren(n, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::VoidType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("UnaryExpression"));
        object->Set(String::New("operator"), String::New("void"));
        object->Set(String::New("argument"), convertChild(n, 0, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::WhileStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("WhileStatement"));
        object->Set(String::New("test"), convertChild(n, 0, indent + 1));
        object->Set(String::New("body"), convertChild(n, 1, indent + 1));
        return object;
    }

    if (n->type() == JSC::SyntaxTree::Node::WithStatementType) {
        Handle<Object> object = Object::New();
        object->Set(String::New("type"), String::New("WithStatement"));
        object->Set(String::New("object"), convertChild(n, 0, indent + 1));
        object->Set(String::New("body"), convertChild(n, 1, indent + 1));
        return object;
    }

    return Undefined();
}

static Handle<Value> reflect_parse(const Arguments& args)
{
    if (args.Length() != 1)
        return ThrowException(String::New("Exception: Reflect.parse() accepts 1 argument"));

    String::Utf8Value code(args[0]);
    UChar *content = new UChar[code.length()];
    for (int i = 0; i < code.length(); ++i)
        content[i] = (*code)[i];
    JSC::UString scriptCode = JSC::UString(content, code.length());
    delete [] content;

    JSC::JSGlobalData* globalData = new JSC::JSGlobalData;
    JSC::SyntaxTree::Node* tree = reinterpret_cast<JSC::SyntaxTree::Node*>(globalData->parser->createSyntaxTree(globalData, JSC::makeSource(scriptCode)));
    Handle<Value> result = tree ? convertNode(tree, 0) : ThrowException(String::New("Exception: Reflect.parse error!"));
    delete globalData;
    return result;
}

void setup_Reflect(Handle<Object> object, Handle<Array> args)
{
    Handle<FunctionTemplate> reflectObject = FunctionTemplate::New();

    reflectObject->Set(String::New("parse"), FunctionTemplate::New(reflect_parse)->GetFunction());

    object->Set(String::New("Reflect"), reflectObject->GetFunction());
}
