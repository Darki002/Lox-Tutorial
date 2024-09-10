using System.Globalization;
using Lox.Errors;

namespace Lox;

public class Interpreter : Expr.IVisitor<object?>
{
    public void Interpret(Expr expr)
    {
        try
        {
            var value = Evaluate(expr);
            Console.WriteLine(Stringify(value));
        }
        catch (RuntimeError e)
        {
            Lox.RuntimeError(e);
        }
    }
    
    public object? VisitBinaryExpr(Expr.Binary expr)
    {
        var right = Evaluate(expr.Right);
        var left = Evaluate(expr.Left);

        switch (expr.Operator.Type)
        {
            case TokenType.STAR:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! * (double)left!;
            case TokenType.SLASH:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! / (double)left!;
            case TokenType.MINUS:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! - (double)left!;
            case TokenType.PLUS:
                if (right is double rightNum && left is double leftNum) return rightNum + leftNum;
                if(right is string rightStr && left is string leftStr) return rightStr + leftStr;
                throw new RuntimeError(expr.Operator, "Operands must be two numbers or two strings.");
            case TokenType.GREATER:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! > (double)left!;
            case TokenType.GREATER_EQUAL:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! >= (double)left!;
            case TokenType.LESS:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! < (double)left!;
            case TokenType.LESS_EQUAL:
                CheckNumberOperand(expr.Operator, right, left);
                return (double)right! <= (double)left!;
            case TokenType.EQUAL_EQUAL:
                return IsEqual(right, left);
            case TokenType.BANG_EQUAL:
                return !IsEqual(right, left);
            default:
                throw new ArgumentOutOfRangeException(nameof(expr.Operator.Type));
        }

        // Unreachable
        return null;
    }

    public object? VisitGroupingExpr(Expr.Grouping expr)
    {
        return Evaluate(expr.Expression);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.Value;
    }

    public object? VisitUnaryExpr(Expr.Unary expr)
    {
        var right = Evaluate(expr.Right);

        switch (expr.Operator.Type)
        {
            case TokenType.BANG:
                return !IsTruthy(right);
            case TokenType.MINUS:
                CheckNumberOperand(expr.Operator, right);
                return -(double)right!;
            default:
                return null; // Unreachable (Unary always have BANG or MINUS as the Operator)
        }
    }

    private object? Evaluate(Expr expr)
    {
        return expr.Accept(this);
    }
    
    private static bool IsTruthy(object? right)
    {
        if (right is null) return false;
        if (right is bool boolean) return boolean;

        
        
        return true;
    }
    
    private static bool IsEqual(object? right, object? left)
    {
        if (right is null && left is null) return true;
        if (right is null) return false;
        return right.Equals(left);
    }
    
    private static void CheckNumberOperand(Token @operator, object? operand) {
        if (operand is double) return;
        throw new RuntimeError(@operator, "Operand must be a number.");
    }
    
    private static void CheckNumberOperand(Token @operator, object? right, object? left) {
        if (right is double && left is double) return;
        throw new RuntimeError(@operator, "Operand must be a number.");
    }
    
    private string? Stringify(object? obj) {
        if (obj == null) return "nil";

        if (obj is double num) {
            var text = num.ToString(CultureInfo.InvariantCulture);
            if (text.EndsWith(".0")) {
                text = text.Substring(0, text.Length - 2);
            }
            return text;
        }

        return obj.ToString();
    }
}