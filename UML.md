
classDiagram
    class Person {
        <<abstract>>
        -string name
        +display() virtual
    }

    class Student {
        -string studentId
        -string course
        -double marks
        +display()
        +grade()
    }

    class StudentPortal {
        -vector~Student~ students
        +addStudent()
        +displayStudents()
        +searchStudent()
        +updateStudent()
        +deleteStudent()
        +saveRecords()
        +loadRecords()
    }

    Person <|-- Student
    StudentPortal o-- Student
