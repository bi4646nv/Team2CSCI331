This project processes Zip Code and Place Name data from a CSV file, organizing it by state and finding:
- Easternmost Zip Code/Place Name (smallest longitude)
- Westernmost Zip Code/Place Name (largest longitude)
- Northernmost Zip Code/Place Name (largest latitude)
- Southernmost Zip Code/Place Name (smallest latitude)

The results are written to:
- SortedLocations.txt (formatted text output)
- SortedLocations.csv (CSV output)

By default, the program uses `us_postal_codes.csv` as the input file.

Compile with this command:
`g++ -o location_processor main.cpp Buffer.cpp`
Run with this command:
`./location_processor`

Upon running, the user will be prompted like this:

`Do you want to sort by Zip Code (Z) or Place Name (P): `

This allows for the user to specify whether the data should be sorted by Zip Code or Place Name.

If the user does not enter 'z' or 'p' (not case sensitive), the program will continue prompting the user.

If there is missing data in the input CSV file, the user will recieve warnings like this:

`Warning: Missing county on line: 96940,Palau,PW,,7.2257,134.3622`

`Warning: Missing county on line: 96941,Pohnpei,FM,,7.1383,151.5031`

...

Once the program is finished, a termination message will be produced:

`Output written to SortedLocations.txt and SortedLocations.csv`

Documentation done through Doxygen can be seen in `DoxygenPDF.pdf`