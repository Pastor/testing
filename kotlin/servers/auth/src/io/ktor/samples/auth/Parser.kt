package io.ktor.samples.auth

import java.io.File

object Parser {
    private val students = mutableListOf<Student>()

    data class Student(
        val group: String,
        val number: String,
        val firstName: String,
        val secondName: String,
        val email: String,
        val variant: Int
    )

    fun parse(fileName: String): Parser {
        File(fileName).readLines().forEach {
            val parts = it.split(",")
            students.add(Student(parts[0], parts[1], parts[3], parts[4], parts[5], variant(parts[1])))
        }
        return this
    }

    private fun variant(number: String): Int {
        var sum = 0
        number.toCharArray().filter { it in '0'..'9' }.forEach {
            sum += (it - '0')
        }
        sum %= 22
        if (sum == 0)
            return 1
        return sum
    }

    private fun groups(): List<String> {
        val groups = mutableSetOf<String>()
        students.forEach { groups.add(it.group) }
        return groups.distinct()
    }

    private fun filter(group: String): List<Student> = students.filter { group.contentEquals(it.group) }

    fun print() {
        val groups = groups()
        for (group in groups) {
            val filter = filter(group)
            println("Группа $group")
            for (s in filter) {
                println("${s.firstName}, ${s.secondName}, ${s.number}, ${s.variant}")
            }
        }
    }

    operator fun iterator() = students.iterator()
}

fun main() {
    Parser.parse("E:\\GitHub\\eisgs\\common-eauth\\.temporary\\courseid_6578_participants.csv")
    Parser.print()
}