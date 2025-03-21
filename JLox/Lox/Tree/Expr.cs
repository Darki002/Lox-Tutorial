﻿namespace Lox.Tree;

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

	public record Call(Expr Callee, Token Paren, List<Expr> Arguments) : Expr
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitCallExpr(this);
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
		T VisitUnaryExpr (Unary expr);
		T VisitCallExpr (Call expr);
		T VisitVariableExpr (Variable expr);
		T VisitFunctionExpr (Function expr);
	}
}
