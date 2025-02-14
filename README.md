 Requirement: Ensure csv_process.cpp generates the required output
 
The program should print a table with state-wise zip codes for the Easternmost, Westernmost, Northernmost, and Southernmost locations.


Compile and Run
g++ -o csv_process csv_process.cpp Buffer.cpp   /* First copy paste this and enter, if you get no errors perfect go ahead and copy paste the next code.*/


./csv_process   /*This should list a load of data, which shows it is working*/


Step 4: Generate the Same Output When Sorted by a Different Column
✅ Requirement: Run the program using place_name instead of zip_code



Compile and Run  ( Run it the same way as described above)

g++ -o csv_process_place csv_process.cpp Buffer.cpp

./csv_process_place


Step 5: Verify Data Consistency
✅ Requirement: Ensure csv_verify.cpp correctly validates the data
csv_verify.cpp should ensure that sorting by zip_code and place_name produces consistent results.

Compile and Run

g++ -o csv_verify csv_verify.cpp Buffer.cpp

./csv_verify


Step 6: Generate the Required Script Files

✅ Requirement: Capture terminal output for submission
You need to generate script files for both sorting methods.

Run and Save Output (for sorting by ZIP)

script script_sorted_by_zip.txt
./csv_process
exit

Run and Save Output (for sorting by PLACE)

script script_sorted_by_place.txt

./csv_process_place
exit

Step 7: Zip All ( Once all is checked and verified, we will push this to the main.cpp and then download to Zip and submit)
zip project_submission.zip Buffer.cpp Buffer.h csv_process.cpp csv_verify.cpp main.cpp script_sorted_by_zip.txt script_sorted_by_place.txt us_postal_codes.csv
Final Checklist
✅ CSV file (us_postal_codes.csv) created from XLSX
✅ Buffer.h and Buffer.cpp correctly process the CSV
✅ csv_process.cpp generates required output (sorted by ZIP)
✅ Modified csv_process.cpp generates identical output sorted by PLACE
✅ csv_verify.cpp ensures data consistency
✅ Script files (script_sorted_by_zip.txt and script_sorted_by_place.txt) generated

What is left for now 
Deoxygen file 
All required files are zipped for submission
