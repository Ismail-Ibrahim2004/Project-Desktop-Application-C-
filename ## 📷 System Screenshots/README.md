\# ☕ Café Manager - Reports \& Analytics



نظام إدارة الكافيهات مع لوحة تقارير وتحليلات شاملة باستخدام Qt 6 و SQL Server.



\## 📋 المميزات



\- ✨ واجهة مستخدم عصرية وأنيقة

\- 📊 لوحة إحصائيات حية (Daily Revenue, Orders, Items Sold)

\- 📈 رسوم بيانية تفاعلية:

&#x20; - Line Chart للإيرادات اليومية

&#x20; - Pie Chart للمبيعات حسب الفئة

&#x20; - Bar Chart للطلبات الشهرية

\- 🏆 قائمة Top 5 منتجات مبيعاً

\- 💰 مقارنة بين اليوم والأمس

\- 🎨 تصميم متجاوب مع تأثيرات Shadow



\## 🛠️ التقنيات المستخدمة



\- \*\*Qt 6.11\*\* - إطار العمل

\- \*\*C++17\*\* - لغة البرمجة

\- \*\*Qt Charts\*\* - الرسوم البيانية

\- \*\*SQL Server 2022\*\* - قاعدة البيانات

\- \*\*CMake\*\* - نظام البناء

\- \*\*ODBC Driver 17\*\* - الاتصال بقاعدة البيانات



\## 📦 المتطلبات



\- Qt 6.11 أو أحدث (مع Qt Charts)

\- Microsoft SQL Server 2019 أو أحدث

\- ODBC Driver 17 for SQL Server

\- MinGW 64-bit Compiler

\- Windows 10/11



\## 🚀 طريقة التشغيل



\### 1. إعداد قاعدة البيانات



افتح Command Prompt واكتب:



\\`\\`\\`bash

sqlcmd -S localhost -E -I -i database\_setup.sql

\\`\\`\\`



\### 2. تشغيل المشروع



1\. افتح المشروع في Qt Creator

2\. اضغط \*\*Build → Run CMake\*\*

3\. اضغط \*\*Run\*\* ▶️



\## 📂 هيكل المشروع



\\`\\`\\`

CafeReportsUN/

├── main.cpp                  # نقطة بداية البرنامج

├── mainwindow.h/cpp          # النافذة الرئيسية والـ Sidebar

├── reportspage.h/cpp         # شاشة التقارير

├── databasemanager.h/cpp     # إدارة قاعدة البيانات

├── reportsservice.h/cpp      # جلب البيانات من الداتابيز

├── database\_setup.sql        # سكريبت إنشاء قاعدة البيانات

└── CMakeLists.txt            # إعدادات البناء

\\`\\`\\`



\## 👨‍💻 المطور



تم تطوير المشروع كجزء من نظام إدارة الكافيهات.



\## 📄 الترخيص



MIT License

