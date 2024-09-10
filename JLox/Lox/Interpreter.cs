namespace Lox;

public class Interpreter : Expr.IVisitor<object?>
{
    public object? VisitBinaryExpr(Expr.Binary expr)
    {
        var right = Evaluate(expr.Right);
        var left = Evaluate(expr.Left);

        switch (expr.Operator.Type)
        {
            case TokenType.STAR:
                return (double)right! * (double)left!;
            case TokenType.SLASH:
                return (double)right! / (double)left!;
            case TokenType.MINUS:
                return (double)right! - (double)left!;
            case TokenType.PLUS:
                if (right is double rightNum && left is double leftNum) return rightNum + leftNum;
                if(right is string rightStr && left is string leftStr) return rightStr + leftStr;
                break;
            case TokenType.GREATER:
                return (double)right! > (double)left!;
            case TokenType.GREATER_EQUAL:
                return (double)right! >= (double)left!;
            case TokenType.LESS:
                return (double)right! < (double)left!;
            case TokenType.LESS_EQUAL:
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

        return expr.Operator.Type switch
        {
            TokenType.BANG => !IsTruthy(right),
            TokenType.MINUS => -(double)right!,
            _ => null // Unreachable (Unary always have BANG or MINUS as the Operator)
        };
    }

    private object? Evaluate(Expr expr)
    {
        return expr.Accept(this);
    }
    
    private bool IsTruthy(object? right)
    {
        if (right is null) return false;
        if (right is bool boolean) return boolean;

        
        
        return true;
    }
    
    private bool IsEqual(object? right, object? left)
    {
        if (right is null && left is null) return true;
        if (right is null) return false;
        return right.Equals(left);
    }
}