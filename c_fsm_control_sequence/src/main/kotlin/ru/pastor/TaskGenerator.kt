package ru.pastor

import java.io.FileOutputStream
import java.io.PrintStream
import java.util.*
import kotlin.collections.HashMap
import kotlin.random.Random

val DELAYS: IntArray = intArrayOf(60, 78, 45, 33, 70)
val TIMES: IntArray = intArrayOf(120, 45, 60, 30, 56)

class TaskGenerator private constructor(builder: Builder) {
    private val steps = builder.steps
    private val cylinders = builder.cylinders
    private val errors = builder.errors
    private val times: MutableMap<Int, MutableList<Int>> = hashMapOf()
    private val delays: MutableMap<Int, MutableList<Int>> = hashMapOf()

    fun generate(): Step {
        val parent = Step.parent(cylinders)
        var current = parent
        for (step in 1..steps) {
            val changeCount = RANDOM.nextInt(cylinders)
            val changes = hashMapOf<Int, Boolean>()
            val next = Step(current, cylinders = HashMap(current.cylinders), next = null)
            next.cylinders.putAll(current.changes)
            for (change in 0..changeCount) {
                var changeIndex: Int
                do {
                    changeIndex = RANDOM.nextInt(1, cylinders + 1)
                } while (changes.containsKey(changeIndex))
                changes[changeIndex] = !next[changeIndex]!!
            }
            next.changes.putAll(changes)
            current.next = next
            current = next
        }

        val changeErrors: MutableSet<Int> = mutableSetOf()
        for (error in 1..errors) {
            var changeIndex: Int
            do {
                changeIndex = RANDOM.nextInt(cylinders)
            } while (changeErrors.contains(changeIndex))
            changeErrors.add(changeIndex)
            val step = count(parent, changeIndex)
            step.error = RANDOM.nextInt(1, steps)
        }

        times.clear()
        delays.clear()
        for (step in 0 until steps) {
            val changeDelayIndex = RANDOM.nextInt(DELAYS.size)
            val changeTimeIndex = RANDOM.nextInt(TIMES.size)
            times.computeIfAbsent(TIMES[changeTimeIndex]) { arrayListOf() }.add(step)
            delays.computeIfAbsent(DELAYS[changeDelayIndex]) { arrayListOf() }.add(step)
        }
        return parent
    }

    private fun count(step: Step, index: Int): Step {
        var next: Step? = step
        for (i in 1..index) {
            next = next?.next
        }
        if (next == null)
            return step
        return next
    }

    fun printDelayTimes(stream: PrintStream) {
        stream.println("\\begin{cases}")
        delays.forEach { (k, v) ->
            kotlin.run {
                for ((i, c) in v.withIndex()) {
                    if (i % 8 == 0) {
                        stream.print("\\\\ \n")
                    }
                    stream.print("d_{$c} = ")
                }
                stream.println("$k \\\\")
            }
        }
        stream.println("\\end{cases}")
    }

    fun printStateTimes(stream: PrintStream) {
        stream.println("\\begin{cases}")
        times.forEach { (k, v) ->
            kotlin.run {
                for ((i, c) in v.withIndex()) {
                    if (i % 8 == 0) {
                        stream.print("\\\\ \n")
                    }
                    stream.print("t_{$c} = ")
                }
                stream.println("$k \\\\")
            }
        }
        stream.println("\\end{cases}")
    }


    fun printCylinders(stream: PrintStream) {
        for (c in 1..cylinders) {
            if (c > 1)
                stream.print(", ")
            stream.print("y_$c")
        }
    }

    data class Step(
        val parent: Step?,
        val cylinders: MutableMap<Int, Boolean> = hashMapOf(),
        val changes: MutableMap<Int, Boolean> = hashMapOf(),
        var error: Int? = null,
        var delay: Int? = null,
        var time: Int? = null,
        var next: Step?
    ) {
        companion object {
            fun parent(cylinders: Int): Step {
                val map = hashMapOf<Int, Boolean>()
                for (c in 1..cylinders) {
                    map[c] = false
                }
                return Step(null, map, map, null, null, null, null)
            }
        }

        operator fun get(i: Int): Boolean? {
            return cylinders[i]
        }

        fun printErrors(stream: PrintStream, index: Int, printDot: Boolean, printed: Int = 0) {
            var hasDot = printDot
            var printedElement = printed
            if (error != null) {
                if (printed % 4 == 0) {
                    stream.print("\\\\ \n")
                }
                if (printDot)
                    stream.print(", ")
                if (error == index) {
                    stream.print("(p_{$index}, p_1)")
                } else {
                    stream.print("(p_{$index}, p_{$error})")
                }
                hasDot = true
                ++printedElement
            }
            next?.printErrors(stream, index + 1, hasDot, printedElement)
        }


        fun printSteps(stream: PrintStream, n: Int) {
            stream.print("(")
            var i = 0
            changes.forEach { (k, v) ->
                run {
//                    if (i > 0) {
//                        stream.print(", ")
//                    }
                    if (!v) {
                        stream.print("\\overline{")
                        stream.print("y_{$k}")
                        stream.print("}")
                    } else {
                        stream.print("y_{$k}")
                    }
                }
                ++i
            }
            stream.print(")")
            if (next != null) {
                stream.print(", ")
                if (n % 2 == 0) {
                    stream.print("\\\\ \n")
                }
                next?.printSteps(stream, n + 1)
            }
        }
    }

    data class Builder(
        var cylinders: Int = 8,
        var steps: Int = RANDOM.nextInt(16, 18),
        var errors: Int = RANDOM.nextInt(1, steps / 2),
        var delays: Int = steps,
        var times: Int = steps
    ) {
        fun steps(steps: Int) = apply { this.steps = steps }
        fun errors(errors: Int) = apply { this.errors = errors }
        fun cylinders(cylinders: Int) = apply { this.cylinders = cylinders }
        fun build() = TaskGenerator(this)
    }

    companion object {
        val RANDOM: Random = Random(Date().time)
    }
}

fun equation(type: String, stream: PrintStream, block: (PrintStream) -> Unit) {
    stream.println("\\begin{$type*}")
    block(stream)
    stream.println("\\end{$type*}")
}

fun main() {
    val generator = TaskGenerator.Builder().build()

    val output = PrintStream(FileOutputStream("questions.tex"))
    for (z in 1..208) {
        output.println("\\newpage")
        output.println("\\subsection{Задание №$z}")
        output.println()
        val parent = generator.generate()
        equation("equation", output) {
            it.print("Y = ")
            generator.printCylinders(it)
        }
        output.println()
        equation("dmath"/*multiline*/, output) {
            it.print("P = ")
            parent.printSteps(it, 1)
            it.println()
        }
        output.println()
        equation("dmath", output) {
            it.print("T = ")
            generator.printStateTimes(it)
        }
        output.println()
        equation("dmath", output) {
            it.print("D = ")
            generator.printDelayTimes(it)
        }
        output.println()
        equation("equation", output) {
            it.print("E = {")
            parent.printErrors(it, 1, false)
            it.println("}. \n")
        }
        output.println()
        output.println()
    }

}