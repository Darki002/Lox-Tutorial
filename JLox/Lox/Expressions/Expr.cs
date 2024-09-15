// ReSharper disable once CheckNamespace
namespace Lox;

public abstract record Expr
{
	public record Assign(Token Name, Expr Value) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitAssignExpr(this);
		}
	}

	public record Binary(Expr Left, Token Operator, Expr Right) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitBinaryExpr(this);
		}
	}

	public record Grouping(Expr Expression) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitGroupingExpr(this);
		}
	}

	public record Literal(object? Value) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitLiteralExpr(this);
		}
	}

	public record Logical(Expr Left, Token Operator, Expr Right) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitLogicalExpr(this);
		}
	}

	public record Unary(Token Operator, Expr Right) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitUnaryExpr(this);
		}
	}

	public record Variable(Token Name) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitVariableExpr(this);
		}
	}

	public abstract T Accept<T>(IVisitor<T> visitor);

	public interface IVisitor<out T>
	{
		T VisitAssignExpr (Assign expr);
		T VisitBinaryExpr (Binary expr);
		T VisitGroupingExpr (Grouping expr);
		T VisitLiteralExpr (Literal expr);
		T VisitLogicalExpr (Logical expr);
		T VisitUnaryExpr (Unary expr);
		T VisitVariableExpr (Variable expr);
	}
}
