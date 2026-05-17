-- =========================================================
-- DATABASE : CafeManagerDB
-- DESCRIPTION : Cafe Management System Database
-- =========================================================

-- =========================================================
-- CREATE DATABASE
-- =========================================================

CREATE DATABASE CafeManagerDB;
GO

USE CafeManagerDB;
GO

-- =========================================================
-- TABLE : Users
-- =========================================================

CREATE TABLE users (
    user_id INT IDENTITY(1,1) PRIMARY KEY,
    username NVARCHAR(50) NOT NULL UNIQUE,
    password NVARCHAR(100) NOT NULL,
    role NVARCHAR(20) NOT NULL 
        CHECK (role IN ('Admin', 'Employee')),
    full_name NVARCHAR(100),
    is_active BIT DEFAULT 1,
    created_at DATETIME DEFAULT GETDATE()
);

-- =========================================================
-- TABLE : Employees
-- =========================================================

CREATE TABLE Employees (
    EmployeeID INT IDENTITY(1,1) PRIMARY KEY,

    user_id INT UNIQUE,

    EmployeeName NVARCHAR(100) NOT NULL,
    Role NVARCHAR(50) DEFAULT 'Employee',

    IsActive BIT DEFAULT 1,
    IsWorking BIT DEFAULT 0,

    Phone NVARCHAR(20),
    Email NVARCHAR(100),

    HireDate DATE DEFAULT GETDATE(),
    CreatedAt DATETIME DEFAULT GETDATE(),

    CONSTRAINT FK_Employees_Users
        FOREIGN KEY (user_id)
        REFERENCES users(user_id)
);

-- =========================================================
-- TABLE : Products
-- =========================================================

CREATE TABLE Products (
    ProductID INT IDENTITY(1,1) PRIMARY KEY,

    ProductName NVARCHAR(100) NOT NULL,
    Price DECIMAL(10,2) NOT NULL,

    Category NVARCHAR(50),

    IsAvailable BIT DEFAULT 1,
    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =========================================================
-- TABLE : Orders
-- =========================================================

CREATE TABLE Orders (
    OrderID INT IDENTITY(1,1) PRIMARY KEY,

    EmployeeID INT NOT NULL,

    OrderDate DATE NOT NULL 
        DEFAULT CAST(GETDATE() AS DATE),

    OrderTime TIME NOT NULL 
        DEFAULT CAST(GETDATE() AS TIME),

    Total DECIMAL(10,2) NOT NULL DEFAULT 0,

    Status NVARCHAR(20) DEFAULT 'pending',

    CreatedAt DATETIME DEFAULT GETDATE(),

    CONSTRAINT FK_Orders_Employees
        FOREIGN KEY (EmployeeID)
        REFERENCES Employees(EmployeeID)
);

-- =========================================================
-- TABLE : OrderDetails
-- =========================================================

CREATE TABLE OrderDetails (
    OrderDetailID INT IDENTITY(1,1) PRIMARY KEY,

    OrderID INT NOT NULL,
    ProductID INT NOT NULL,

    Quantity INT NOT NULL DEFAULT 1,

    UnitPrice DECIMAL(10,2) NOT NULL,

    Subtotal AS (Quantity * UnitPrice),

    CONSTRAINT FK_OrderDetails_Orders
        FOREIGN KEY (OrderID)
        REFERENCES Orders(OrderID)
        ON DELETE CASCADE,

    CONSTRAINT FK_OrderDetails_Products
        FOREIGN KEY (ProductID)
        REFERENCES Products(ProductID)
);

-- =========================================================
-- TABLE : Inventory
-- =========================================================

CREATE TABLE Inventory (
    InventoryID INT IDENTITY(1,1) PRIMARY KEY,

    ItemName NVARCHAR(100) NOT NULL,

    Quantity DECIMAL(10,2) NOT NULL DEFAULT 0,

    Unit NVARCHAR(20),

    MinQuantity DECIMAL(10,2) DEFAULT 0,

    Supplier NVARCHAR(100),

    LastRestocked DATETIME,

    CreatedAt DATETIME DEFAULT GETDATE()
);

-- =========================================================
-- TABLE : ProductIngredients
-- =========================================================

CREATE TABLE ProductIngredients (
    IngredientID INT IDENTITY(1,1) PRIMARY KEY,

    ProductID INT NOT NULL,
    InventoryID INT NOT NULL,

    QuantityRequired DECIMAL(10,2) NOT NULL,

    CONSTRAINT FK_ProductIngredients_Products
        FOREIGN KEY (ProductID)
        REFERENCES Products(ProductID),

    CONSTRAINT FK_ProductIngredients_Inventory
        FOREIGN KEY (InventoryID)
        REFERENCES Inventory(InventoryID)
);

-- =========================================================
-- TABLE : CafeSettings
-- =========================================================

CREATE TABLE CafeSettings (
    SettingKey NVARCHAR(50) PRIMARY KEY,
    SettingValue NVARCHAR(500)
);

-- =========================================================
-- SAMPLE DATA : USERS
-- =========================================================

INSERT INTO users 
(username, password, role, full_name)
VALUES
('admin',   '123', 'Admin',    'Administrator'),
('manager', '123', 'Admin',    'Cafe Manager'),
('john',    '123', 'Employee', 'John Doe'),
('sara',    '123', 'Employee', 'Sara Ahmed'),
('mike',    '123', 'Employee', 'Mike Johnson');

-- =========================================================
-- SAMPLE DATA : EMPLOYEES
-- =========================================================

INSERT INTO Employees
(user_id, EmployeeName, Role, IsActive, IsWorking)
VALUES
(1, 'John Doe',      'Manager', 1, 1),
(2, 'Jane Smith',    'Barista', 1, 1),
(3, 'Mike Johnson',  'Cashier', 1, 1),
(4, 'Sarah Brown',   'Waiter',  1, 1),
(5, 'Tom Wilson',    'Chef',    1, 0);

-- =========================================================
-- SAMPLE DATA : PRODUCTS
-- =========================================================

INSERT INTO Products
(ProductName, Price, Category)
VALUES
('Cappuccino',    4.50, 'Coffee'),
('Latte',         5.00, 'Coffee'),
('Espresso',      3.50, 'Coffee'),
('Americano',     3.00, 'Coffee'),
('Mocha',         5.50, 'Coffee'),
('Hot Chocolate', 4.00, 'Beverages'),
('Green Tea',     3.50, 'Tea'),
('Croissant',     3.00, 'Pastry'),
('Muffin',        2.50, 'Pastry'),
('Cheesecake',    6.00, 'Dessert');

-- =========================================================
-- SAMPLE DATA : ORDERS
-- =========================================================

INSERT INTO Orders
(EmployeeID, OrderDate, OrderTime, Total, Status)
VALUES
(1, '2026-03-07', '10:30:00', 24.50, 'completed'),
(2, '2026-03-07', '11:15:00', 18.75, 'completed'),
(3, '2026-03-07', '11:45:00', 32.00, 'completed');

-- =========================================================
-- SAMPLE DATA : ORDER DETAILS
-- =========================================================

INSERT INTO OrderDetails
(OrderID, ProductID, Quantity, UnitPrice)
VALUES
(1, 1, 2, 4.50),
(1, 8, 1, 3.00),
(2, 2, 1, 5.00),
(2, 5, 1, 5.50),
(3, 1, 3, 4.50);

-- =========================================================
-- SAMPLE DATA : INVENTORY
-- =========================================================

INSERT INTO Inventory
(ItemName, Quantity, Unit, MinQuantity, Supplier)
VALUES
('Coffee Beans', 25, 'kg', 10, 'Bean Masters'),
('Milk', 45, 'liters', 15, 'Dairy Fresh'),
('Sugar', 8, 'kg', 10, 'Sweet Supply');

-- =========================================================
-- SAMPLE DATA : PRODUCT INGREDIENTS
-- =========================================================

INSERT INTO ProductIngredients
(ProductID, InventoryID, QuantityRequired)
VALUES
(1, 1, 0.25),
(1, 2, 0.10),
(2, 1, 0.20),
(2, 2, 0.15);

-- =========================================================
-- SAMPLE DATA : CAFE SETTINGS
-- =========================================================

INSERT INTO CafeSettings
VALUES
('CafeName', 'Cafe Manager'),
('Phone', '(555) 123-4567'),
('Currency', 'USD'),
('TaxRate', '8');