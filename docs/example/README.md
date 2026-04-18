# Example

example upload and output

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [example upload](#example-upload)
- [example output (test)](#example-output-test)
- [example uploaded JSON data](#example-uploaded-json-data)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## example upload

```bash
./build/json_uploader  --json docs/example/hr_test-data.json --schema docs/example/hr_schema_v1.json --env-path docs/example/hr_test.env

[2026-04-18 13:09:14.484] [info] JSON Uploader starting...
[2026-04-18 13:09:14.487] [info] Authenticating with API at http://localhost:18080/login
[2026-04-18 13:09:14.488] [info] Authentication successful
[2026-04-18 13:09:14.488] [info] Starting upload of data/hr_test-daten.json (Compression: NONE)
[2026-04-18 13:09:14.488] [info] Loading schema from data/hr_schema_v1.json
[2026-04-18 13:09:14.536] [info] Upload completed successfully
```

## example output (test)

```bash
tests/build/crow_test_server --user admin --password change_me --port 18080

Starting Crow-Server on port 18080
(2026-04-18 10:41:59) [INFO    ] Crow/master server is running at http://0.0.0.0:18080 using 6 threads
(2026-04-18 10:41:59) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.
(2026-04-18 10:45:43) [INFO    ] Request: 127.0.0.1:41468 0x60bf11ebaff0 HTTP/1.1 POST /login
(2026-04-18 10:45:43) [INFO    ] Response: 0x60bf11ebaff0 /login 200 0
(2026-04-18 10:45:43) [INFO    ] Request: 127.0.0.1:41474 0x7349c8000ca0 HTTP/1.1 POST /upload
```

## example uploaded JSON data

```json
[
  {
    "authentication": {
      "role": "uploader",
      "password": "topsecret",
      "username": "Robert"
    }
  },
  {
    "Building.Code": "B-001",
    "Building.Description": "Main Office",
    "CellPhone": "+49-171-5551234",
    "DateOfBirth": "1985-04-12",
    "DateOfHire": "2010-09-01",
    "DateOfTermination": null,
    "Department.Code": "DEP10",
    "Department.Description": "Engineering",
    "Division.Code": "DIV-A",
    "Division.Description": "Product Development",
    "DriverLicenseClass": "B",
    "DriverLicenseNumber": "DLN1234567",
    "DriverLicenseState.Code": "BY",
    "DriverLicenseState.Description": "Bayern",
    "Email": "employee@example.com",
    "EmergencyContact": "Anna Musterfrau",
    "EmergencyHomePhone": "+49-89-123456",
    "EmergencyRelation": "Spouse",
    "EmergencyWorkPhone": "+49-89-987654",
    "EmployeeNumber": "EMP-1001",
    "EmployeeType.Code": "FULL",
    "EmployeeType.Description": "Full-time",
    "EthnicGroup.Code": "EG1",
    "EthnicGroup.Description": "Group 1",
    "FirstName": "Max",
    "FloorArea.Code": "FA-01",
    "FloorArea.Description": "North Wing",
    "Gender": 1,
    "GeographicLocation.Code": "GL-DE-BY",
    "GeographicLocation.Description": "Germany / Bavaria",
    "HealthCenter.Code": "HC01",
    "HealthCenter.Description": "Health Center Munich",
    "HealthInsurance.Code": "HI-AOK",
    "HealthInsurance.Description": "AOK Bayern",
    "HipaaAuth": true,
    "HipaaAuthDate": "2020-01-15",
    "HomeCity": "Moosburg",
    "HomeCountry.Code": "DE",
    "HomeCountry.Description": "Deutschland",
    "HomePhone": "+49-8761-12345",
    "HomeState.Code": "BY",
    "HomeState.Description": "Bayern",
    "HomeStreet1": "Hauptstraße 12",
    "HomeStreet2": "",
    "HomeZip": "85368",
    "HourlyRate": 42.5,
    "JobPosition.Code": "JP-DEV",
    "JobPosition.Description": "Software Developer",
    "JobPositionGroup.Code": "JPG-ENG",
    "JobPositionGroup.Description": "Engineering Group",
    "JobType.Code": "JT-STD",
    "JobType.Description": "Standard",
    "Language.Code": "DE",
    "Language.Description": "Deutsch",
    "LastName": "Mustermann",
    "Location.Code": "LOC-MUC",
    "Location.Description": "Munich HQ",
    "LoginId": "mmustermann",
    "MaidenName": "",
    "MailStop": "MS-01",
    "MaritalStatus.Code": "M",
    "MaritalStatus.Description": "Married",
    "MedicalInsuranceNumber": "MI-123456789",
    "MedicalRecordNumber": "MR-987654",
    "MiddleName": "",
    "MonthlyWorkHours": 160,
    "NickName": "Maxi",
    "Organization.Code": "ORG-01",
    "Organization.Description": "Corporate",
    "Pager": "",
    "Photo": null,
    "Physician": "Dr. Erika Beispiel",
    "PhysicianCity": "Freising",
    "PhysicianPhone": "+49-8161-55555",
    "PhysicianState.Code": "BY",
    "PhysicianState.Description": "Bayern",
    "PhysicianStreet": "Ärzteweg 5",
    "PhysicianZip": "85354",
    "SSNValue": "123-45-6789",
    "SecondaryHealthCenter.Code": "HC02",
    "SecondaryHealthCenter.Description": "Backup Health Center",
    "SeniorityDate": "2015-03-01",
    "ShiftCode.Code": "SHIFT-A",
    "ShiftCode.Description": "Morning Shift",
    "ShiftCode.ShiftBegins": "2024-01-01",
    "ShiftCode.ShiftDuration": 8,
    "Supervisor.EmployeeNumber": "EMP-0001",
    "UDF1": "Custom1",
    "UDF2": "Custom2",
    "UDF3": "Custom3",
    "UDF4": "Custom4",
    "UDF5": "Custom5",
    "Union.Code": "UN-01",
    "Union.Description": "General Workers Union",
    "WorkExtension": "123",
    "WorkPhone": "+49-89-111222",
    "WorkStatus.Code": "ACTIVE",
    "WorkStatus.Description": "Currently employed",
    "authentication": {
      "role": "uploader",
      "password": "topsecret",
      "username": "Robert"
    },
    "metadata": {
      "DEBUG": "yes",
      "environment": "production",
      "region": "eu-central-1",
      "user": "Robert",
      "userpwd": "topsecret"
    },
    "schema_version": 1
  },
  {
    "Building.Code": "B-001",
    "Building.Description": "Hauptbüro",
    "CellPhone": "+49-171-5551234",
    "DateOfBirth": "1985-04-12",
    "DateOfHire": "2010-09-01",
    "DateOfTermination": null,
    "Department.Code": "DEP10",
    "Department.Description": "Ingenieurwesen",
    "Division.Code": "DIV-A",
    "Division.Description": "Produktentwicklung",
    "DriverLicenseClass": "B",
    "DriverLicenseNumber": "DLN1234567",
    "DriverLicenseState.Code": "BY",
    "DriverLicenseState.Description": "Bayern",
    "Email": "employee@example.com",
    "EmergencyContact": "Anna Musterfrau",
    "EmergencyHomePhone": "+49-89-123456",
    "EmergencyRelation": "Ehepartner",
    "EmergencyWorkPhone": "+49-89-987654",
    "EmployeeNumber": "EMP-1001",
    "EmployeeType.Code": "FULL",
    "EmployeeType.Description": "Vollzeit",
    "EthnicGroup.Code": "EG1",
    "EthnicGroup.Description": "Gruppe 1",
    "FirstName": "Max",
    "FloorArea.Code": "FA-01",
    "FloorArea.Description": "Nordflügel",
    "Gender": 1,
    "GeographicLocation.Code": "GL-DE-BY",
    "GeographicLocation.Description": "Deutschland / Bayern",
    "HealthCenter.Code": "HC01",
    "HealthCenter.Description": "Gesundheitszentrum München",
    "HealthInsurance.Code": "HI-AOK",
    "HealthInsurance.Description": "AOK Bayern",
    "HipaaAuth": true,
    "HipaaAuthDate": "2020-01-15",
    "HomeCity": "Moosburg",
    "HomeCountry.Code": "DE",
    "HomeCountry.Description": "Deutschland",
    "HomePhone": "+49-8761-12345",
    "HomeState.Code": "BY",
    "HomeState.Description": "Bayern",
    "HomeStreet1": "Hauptstraße 12",
    "HomeStreet2": "",
    "HomeZip": "85368",
    "HourlyRate": 42.5,
    "JobPosition.Code": "JP-DEV",
    "JobPosition.Description": "Softwareentwickler",
    "JobPositionGroup.Code": "JPG-ENG",
    "JobPositionGroup.Description": "Ingenieurgruppe",
    "JobType.Code": "JT-STD",
    "JobType.Description": "Standard",
    "Language.Code": "DE",
    "Language.Description": "Deutsch",
    "LastName": "Mustermann",
    "Location.Code": "LOC-MUC",
    "Location.Description": "München Zentrale",
    "LoginId": "mmustermann",
    "MaidenName": "",
    "MailStop": "MS-01",
    "MaritalStatus.Code": "M",
    "MaritalStatus.Description": "Verheiratet",
    "MedicalInsuranceNumber": "MI-123456789",
    "MedicalRecordNumber": "MR-987654",
    "MiddleName": "",
    "MonthlyWorkHours": 160,
    "NickName": "Maxi",
    "Organization.Code": "ORG-01",
    "Organization.Description": "Unternehmen",
    "Pager": "",
    "Photo": null,
    "Physician": "Dr. Erika Beispiel",
    "PhysicianCity": "Freising",
    "PhysicianPhone": "+49-8161-55555",
    "PhysicianState.Code": "BY",
    "PhysicianState.Description": "Bayern",
    "PhysicianStreet": "Ärzteweg 5",
    "PhysicianZip": "85354",
    "SSNValue": "123-45-6789",
    "SecondaryHealthCenter.Code": "HC02",
    "SecondaryHealthCenter.Description": "Ersatz-Gesundheitszentrum",
    "SeniorityDate": "2015-03-01",
    "ShiftCode.Code": "SHIFT-A",
    "ShiftCode.Description": "Frühschicht",
    "ShiftCode.ShiftBegins": "2024-01-01",
    "ShiftCode.ShiftDuration": 8,
    "Supervisor.EmployeeNumber": "EMP-0001",
    "UDF1": "Custom1",
    "UDF2": "Custom2",
    "UDF3": "Custom3",
    "UDF4": "Custom4",
    "UDF5": "Custom5",
    "Union.Code": "UN-01",
    "Union.Description": "Allgemeine Arbeitergewerkschaft",
    "WorkExtension": "123",
    "WorkPhone": "+49-89-111222",
    "WorkStatus.Code": "ACTIVE",
    "WorkStatus.Description": "Derzeit beschäftigt",
    "authentication": {
      "role": "uploader",
      "password": "topsecret",
      "username": "Robert"
    },
    "metadata": {
      "DEBUG": "yes",
      "environment": "production",
      "region": "eu-central-1",
      "user": "Robert",
      "userpwd": "topsecret"
    },
    "schema_version": 1
  }
]
```
