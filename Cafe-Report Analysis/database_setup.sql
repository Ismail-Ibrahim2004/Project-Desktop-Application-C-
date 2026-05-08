SET QUOTED_IDENTIFIER ON;
GO

IF NOT EXISTS (SELECT * FROM sys.databases WHERE name = 'CafeManagerDB')
BEGIN
    CREATE DATABASE CafeManagerDB;
END
GO

USE CafeManagerDB;
GO

DROP TABLE IF EXISTS OrderDetails;
DROP TABLE IF EXISTS Orders;
DROP TABLE IF EXISTS Products;
GO

CREATE TABLE Products (
    ProductID INT PRIMARY KEY IDENTITY(1,1),
    ProductName NVARCHAR(100) NOT NULL,
    Category NVARCHAR(50) NOT NULL,
    Price DECIMAL(10,2) NOT NULL,
    Cost DECIMAL(10,2) DEFAULT 0,
    IsActive BIT DEFAULT 1
);
GO

CREATE TABLE Orders (
    OrderID INT PRIMARY KEY IDENTITY(1,1),
    OrderDate DATETIME NOT NULL DEFAULT GETDATE(),
    TotalAmount DECIMAL(10,2) NOT NULL,
    Status NVARCHAR(20) DEFAULT 'Completed'
);
GO

CREATE TABLE OrderDetails (
    DetailID INT PRIMARY KEY IDENTITY(1,1),
    OrderID INT NOT NULL,
    ProductID INT NOT NULL,
    Quantity INT NOT NULL,
    UnitPrice DECIMAL(10,2) NOT NULL,
    Subtotal AS (Quantity * UnitPrice) PERSISTED,
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID) ON DELETE CASCADE,
    FOREIGN KEY (ProductID) REFERENCES Products(ProductID)
);
GO

INSERT INTO Products (ProductName, Category, Price, Cost) VALUES
('Cappuccino', 'Coffee', 4.25, 1.50),
('Latte', 'Coffee', 4.50, 1.60),
('Espresso', 'Coffee', 3.50, 1.20),
('Americano', 'Coffee', 3.75, 1.30),
('Macchiato', 'Coffee', 4.75, 1.70),
('Mocha', 'Coffee', 5.00, 1.80),
('Green Tea', 'Tea', 3.25, 0.80),
('Black Tea', 'Tea', 3.00, 0.75),
('Chai Latte', 'Tea', 4.00, 1.20),
('Iced Coffee', 'Cold Drinks', 4.00, 1.40),
('Iced Tea', 'Cold Drinks', 3.50, 1.00),
('Lemonade', 'Cold Drinks', 3.75, 1.10),
('Smoothie', 'Cold Drinks', 5.50, 2.00),
('Chocolate Cake', 'Desserts', 5.50, 2.20),
('Cheesecake', 'Desserts', 6.00, 2.50),
('Tiramisu', 'Desserts', 6.50, 2.80),
('Croissant', 'Desserts', 3.50, 1.20),
('Muffin', 'Desserts', 3.25, 1.10);
GO

DECLARE @StartDate DATETIME = DATEADD(MONTH, -6, GETDATE());
DECLARE @EndDate DATETIME = GETDATE();
DECLARE @CurrentDate DATETIME = @StartDate;
DECLARE @OrderID INT;
DECLARE @ProductID INT;
DECLARE @Quantity INT;
DECLARE @UnitPrice DECIMAL(10,2);
DECLARE @OrdersPerDay INT;
DECLARE @i INT;
DECLARE @j INT;
DECLARE @ItemsPerOrder INT;
DECLARE @TotalAmount DECIMAL(10,2);

WHILE @CurrentDate <= @EndDate
BEGIN
    SET @OrdersPerDay = 15 + CAST(RAND() * 35 AS INT);
    SET @i = 0;
    
    WHILE @i < @OrdersPerDay
    BEGIN
        INSERT INTO Orders (OrderDate, TotalAmount, Status)
        VALUES (
            DATEADD(MINUTE, CAST(RAND() * 720 AS INT), @CurrentDate),
            0,
            'Completed'
        );
        
        SET @OrderID = SCOPE_IDENTITY();
        SET @TotalAmount = 0;
        SET @ItemsPerOrder = 1 + CAST(RAND() * 4 AS INT);
        SET @j = 0;
        
        WHILE @j < @ItemsPerOrder
        BEGIN
            SELECT TOP 1 @ProductID = ProductID, @UnitPrice = Price
            FROM Products
            ORDER BY NEWID();
            
            SET @Quantity = 1 + CAST(RAND() * 2 AS INT);
            
            INSERT INTO OrderDetails (OrderID, ProductID, Quantity, UnitPrice)
            VALUES (@OrderID, @ProductID, @Quantity, @UnitPrice);
            
            SET @TotalAmount = @TotalAmount + (@Quantity * @UnitPrice);
            SET @j = @j + 1;
        END
        
        UPDATE Orders SET TotalAmount = @TotalAmount WHERE OrderID = @OrderID;
        SET @i = @i + 1;
    END
    
    SET @CurrentDate = DATEADD(DAY, 1, @CurrentDate);
END
GO

PRINT 'Database setup completed successfully!';
GO