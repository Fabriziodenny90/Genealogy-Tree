A genealogy tree is described in a text file with the following format:
• The first row specifies the number n of people in the genealogy tree.
• All subsequent n rows describe a person in the tree with the format:
name surname sex age
where:
– name and surname indicate the first and the last name of the person (string fields, without spaces, of at most
50 characters).
– sex is a single character, where ’M’ stands for male and ’F’ for female.
– age represent the age of the person when he/she died.
• All subsequent rows, unknown in number, specify the relations among those people. Each row has a structure like
nameChild surnameChild nameFather surnameFather nameMother surnameMother
where for each person (first and last name) it is reported the father and mother first and last names.
Notice that, obviously, each person has exactly two parents, but there is no limit to the number of children each person
may have. Moreover, there is no specific order in the file to report child-to-parents relationships.
Write a C program able to:
• Store the file content in a proper data structure.
• Find the path in this data structure with maximum number of people of the the same sex, i.e., find the path that,
following parents-to-child relationships, encounters the maximum number of people with the same sex (male or
female).
• Find the path in this data structure whose sum of the life spans is maximum, i.e., find the path that, following
parents-to-child relationships, encounters people whose sum of the life spans is maximum.