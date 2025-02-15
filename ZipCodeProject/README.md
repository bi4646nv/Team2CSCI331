This project processes zip code data from a CSV file, organizing it by state and finding:
- Easternmost zip code (smallest longitude)
- Westernmost zip code (largest longitude)
- Northernmost zip code (largest latitude)
- Southernmost zip code (smallest latitude)

The results are written to:
- LocationSortedZips.txt (formatted text output)
- LocationSortedZips.csv (CSV output)

By default, it uses `us_postal_codes.csv`, but you can specify a different input file.

Compile with this command:
`g++ -o zipcode_processor main.cpp Buffer.cpp`
Run with this command:
`./zipcode_processor`

Upon running, the user will be prompted like this:
`Default file input: "us_postal_codes.csv"`

`Would you like to change this? (Y/N):`
This allows for the user to specify their input file or to use the default one.

If there is missing data in the input CSV file, the user will recieve warnings like this:
`Warning: Missing county on line: 96940,Palau,PW,,7.2257,134.3622`
`Warning: Missing county on line: 96941,Pohnpei,FM,,7.1383,151.5031`
`...`

If the user does not use a proper input file, they will recieve a message like this:
`Default file input: "us_postal_codes.csv"`
`Would you like to change this? (Y/N): y`
`Enter the input file name: nonexistentfile.csv`
`Error: Could not open the file nonexistentfile.csv`
`Error: Unable to read CSV file: nonexistentfile.csv`