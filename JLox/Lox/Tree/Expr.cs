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

	public record Set(Expr Obj, Token Name, Expr Value) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitSetExpr(this);
		}
	}

	public record IndexSet(Expr Obj, Expr Index, Expr Value, Token Token) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitIndexSetExpr(this);
		}
	}

	public record Super(Token Keyword, Token Method) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitSuperExpr(this);
		}
	}

	public record This(Token Keyword) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitThisExpr(this);
		}
	}

	public record Unary(Token Operator, Expr Right) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitUnaryExpr(this);
		}
	}

	public record Call(Expr Callee, Token Paren, List<Expr> Arguments) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitCallExpr(this);
		}
	}

	public record Get(Expr Obj, Token Name) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitGetExpr(this);
		}
	}

	public record IndexGet(Expr Obj, Expr Index, Token Token) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitIndexGetExpr(this);
		}
	}

	public record Variable(Token Name) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitVariableExpr(this);
		}
	}

	public record Function(List<Token> Params, List<Stmt> Body) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitFunctionExpr(this);
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
		T VisitSetExpr (Set expr);
		T VisitIndexSetExpr (IndexSet expr);
		T VisitSuperExpr (Super expr);
		T VisitThisExpr (This expr);
		T VisitUnaryExpr (Unary expr);
		T VisitCallExpr (Call expr);
		T VisitGetExpr (Get expr);
		T VisitIndexGetExpr (IndexGet expr);
		T VisitVariableExpr (Variable expr);
		T VisitFunctionExpr (Function expr);
	}
}
