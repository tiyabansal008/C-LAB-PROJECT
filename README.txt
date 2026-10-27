LIBRARY MANAGEMENT SYSTEM (C ONLY)

Compile:
  gcc -std=c11 main.c -o library.exe

Run on Windows:
  library.exe

Default administrator credentials:
  Username: ansh
  Password: Library@2026

Book listings now use a dotted-line-free, fixed-width ASCII table with borders,
so the ID, title, author and available quantity columns stay aligned.

Data files are created in the current working directory:
  members.dat, books.dat, loans.dat, activity.dat
Keep these files to preserve records. Registration OTP is displayed in the
console for demonstration only; it does not send an SMS.
