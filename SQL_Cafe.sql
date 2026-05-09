-- =============================================
-- Create Database: CafeManagerDB
-- =============================================

CREATE DATABASE CafeManagerDB;
GO

USE CafeManagerDB;
GO

-- =============================================
-- Table: Employees
-- =============================================
CREATE TABLE Employees (
    EmployeeID INT IDENTITY(1,1) PRIMARY KEY,
    EmployeeName NVARCHAR(100) NOT NULL,
    IsActive BIT DEFAULT 1,
    IsWorking BIT DEFAULT 0,
    Phone NVARCHAR(20),
    Email NVARCHAR(100),
    HireDate DATE DEFAULT GETDATE(),
    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =============================================
-- Table: Products
-- =============================================
CREATE TABLE Products (
    ProductID INT IDENTITY(1,1) PRIMARY KEY,
    ProductName NVARCHAR(100) NOT NULL,
    Price DECIMAL(10,2) NOT NULL,
    Category NVARCHAR(50),
    IsAvailable BIT DEFAULT 1,
    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =============================================
-- Table: Orders
-- =============================================
CREATE TABLE Orders (
    OrderID INT IDENTITY(1,1) PRIMARY KEY,
    EmployeeID INT FOREIGN KEY REFERENCES Employees(EmployeeID),
    OrderDate DATE NOT NULL DEFAULT CAST(GETDATE() AS DATE),
    OrderTime TIME NOT NULL DEFAULT CAST(GETDATE() AS TIME),
    Total DECIMAL(10,2) NOT NULL DEFAULT 0,
    Status NVARCHAR(20) DEFAULT 'pending',
    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =============================================
-- Table: OrderDetails
-- =============================================
CREATE TABLE OrderDetails (
    OrderDetailID INT IDENTITY(1,1) PRIMARY KEY,
    OrderID INT FOREIGN KEY REFERENCES Orders(OrderID) ON DELETE CASCADE,
    ProductID INT FOREIGN KEY REFERENCES Products(ProductID),
    Quantity INT NOT NULL DEFAULT 1,
    UnitPrice DECIMAL(10,2) NOT NULL,
    Subtotal AS (Quantity * UnitPrice)
);

-- =============================================
-- Table: Inventory
-- =============================================
CREATE TABLE Inventory (
    InventoryID INT IDENTITY(1,1) PRIMARY KEY,
    ItemName NVARCHAR(100) NOT NULL,
    Quantity DECIMAL(10,2) NOT NULL DEFAULT 0,
    Unit NVARCHAR(20),
    MinQuantity DECIMAL(10,2) DEFAULT 0,
    LastRestocked DATETIME,
    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =============================================
-- Insert Sample Data
-- =============================================

-- Employees
INSERT INTO Employees (EmployeeName, IsActive, IsWorking) VALUES
('John Doe', 1, 1),
('Jane Smith', 1, 1),
('Mike Johnson', 1, 1),
('Sarah Brown', 1, 1),
('Tom Wilson', 1, 0),
('Emily Davis', 1, 0),
('Robert Garcia', 1, 0),
('Lisa Anderson', 1, 0),
('James Martinez', 1, 0),
('Jennifer Taylor', 1, 0),
('David Thomas', 1, 0),
('Maria Hernandez', 1, 0),
('Richard Moore', 1, 0),
('Susan Jackson', 1, 0),
('Joseph White', 1, 0),
('Margaret Harris', 1, 0),
('Charles Clark', 1, 0),
('Dorothy Lewis', 1, 0),
('Christopher Robinson', 1, 0),
('Nancy Walker', 1, 0),
('Daniel Hall', 1, 0),
('Betty Allen', 1, 0),
('Paul Young', 1, 0),
('Karen King', 1, 0);

-- Products
INSERT INTO Products (ProductName, Price, Category) VALUES
('Cappuccino', 4.50, 'Coffee'),
('Latte', 5.00, 'Coffee'),
('Espresso', 3.50, 'Coffee'),
('Americano', 3.00, 'Coffee'),
('Mocha', 5.50, 'Coffee'),
('Hot Chocolate', 4.00, 'Beverages'),
('Green Tea', 3.50, 'Tea'),
('Croissant', 3.00, 'Pastry'),
('Muffin', 2.50, 'Pastry'),
('Cheesecake', 6.00, 'Dessert');

-- Orders (sample recent orders)
INSERT INTO Orders (EmployeeID, OrderDate, OrderTime, Total, Status) VALUES
(1, '2026-03-07', '10:30:00', 24.50, 'completed'),
(2, '2026-03-07', '11:15:00', 18.75, 'completed'),
(3, '2026-03-07', '11:45:00', 32.00, 'completed'),
(4, '2026-03-07', '12:30:00', 15.25, 'pending'),
(1, '2026-03-07', '13:00:00', 42.50, 'completed');

-- OrderDetails
INSERT INTO OrderDetails (OrderID, ProductID, Quantity, UnitPrice) VALUES
(1, 1, 2, 4.50),
(1, 8, 1, 3.00),
(1, 9, 3, 2.50),
(2, 2, 1, 5.00),
(2, 5, 1, 5.50),
(2, 7, 1, 3.50),
(3, 1, 3, 4.50),
(3, 10, 1, 6.00),
(3, 3, 2, 3.50),
(4, 4, 2, 3.00),
(4, 9, 1, 2.50),
(4, 6, 1, 4.00),
(5, 1, 4, 4.50),
(5, 2, 2, 5.00),
(5, 8, 3, 3.00);

-- Inventory
INSERT INTO Inventory (ItemName, Quantity, Unit, MinQuantity) VALUES
('Coffee Beans', 25.0, 'kg', 5.0),
('Milk', 50.0, 'liters', 10.0),
('Sugar', 15.0, 'kg', 3.0),
('Chocolate Powder', 8.0, 'kg', 2.0),
('Tea Leaves', 5.0, 'kg', 1.0),
('Flour', 20.0, 'kg', 5.0),
('Butter', 10.0, 'kg', 2.0),
('Cream', 15.0, 'liters', 3.0),
('Cups (Small)', 500.0, 'pcs', 100.0),
('Cups (Large)', 400.0, 'pcs', 100.0);

GO
USE CafeManagerDB;
GO

-- ===========================================
-- 🆕 إنشاء جدول users
-- ===========================================
CREATE TABLE users (
    user_id INT IDENTITY(1,1) PRIMARY KEY,
    username NVARCHAR(50) NOT NULL UNIQUE,
    password NVARCHAR(100) NOT NULL,
    role NVARCHAR(20) NOT NULL CHECK (role IN ('Admin', 'Employee')),
    full_name NVARCHAR(100),
    is_active BIT DEFAULT 1,
    created_at DATETIME DEFAULT GETDATE()
);
GO

-- ===========================================
-- 👥 إضافة مستخدمين للاختبار
-- ===========================================
INSERT INTO users (username, password, role, full_name) VALUES
('admin',   '123', 'Admin',    'Administrator'),
('manager', '123', 'Admin',    'Cafe Manager'),
('john',    '123', 'Employee', 'John Doe'),
('sara',    '123', 'Employee', 'Sara Ahmed'),
('mike',    '123', 'Employee', 'Mike Johnson');
GO

-- ===========================================
-- ✅ التحقق
-- ===========================================
SELECT * FROM users;