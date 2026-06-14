0 - move $r32 $r1 # endereço bcp
1 - subi $r1 $r1 70 # endereço bcp
2 - move $r61 $r1   # endereço base do array 'processos'
3 - subi $r1 $r1 10  # próximo elemento
4 - move $r33 $r1   # endereço base do array 'programas'
5 - subi $r1 $r1 10  # próximo elemento
6 - move $r34 $r1   # endereço base do array 'halts'
7 - subi $r1 $r1 10  # próximo elemento
8 - subi $r1 $r1 1 # aloca espaço para variável 'qtdHalt'
9 - sw $r63 0($r1)  # inicializa com 0
10 - move $r35 $r1   # endereço da variável 'qtdHalt'
11 - move $r36 $r1   # endereço base do array 'processosExecucao'
12 - subi $r1 $r1 10  # próximo elemento
13 - move $r37 $r1   # endereço base do array 'dados'
14 - subi $r1 $r1 10  # próximo elemento
15 - move $r38 $r1   # endereço base do array 'rodouumavez'
16 - subi $r1 $r1 10  # próximo elemento
17 - addil $r43 $r44 main
18 - j main
19 - mapeamento: # nova função mapeamento
20 - subi $r1 $r1 1      # aloca espaço na pilha
21 - sw $r31 0($r1)       # empilha registrador
22 - subi $r1 $r1 1      # aloca espaço na pilha
23 - sw $r2 0($r1)       # empilha registrador
24 - move $r2 $r1         # fp = sp
25 - subi $r1 $r1 1 # aloca espaço para variável 'i'
26 - sw $r63 0($r1)  # inicializa com 0
27 - move $r5 $r1   # endereço da variável 'i'
28 - subi $r1 $r1 1 # aloca espaço para variável 'desvio'
29 - sw $r63 0($r1)  # inicializa com 0
30 - move $r6 $r1   # endereço da variável 'desvio'
31 - subi $r1 $r1 1 # aloca espaço para variável 'desviodados'
32 - sw $r63 0($r1)  # inicializa com 0
33 - move $r7 $r1   # endereço da variável 'desviodados'
34 - li $r9 0
35 - sw $r9 0($r5) # movendo t0 para i
36 - li $r9 11000
37 - sw $r9 0($r7) # movendo t1 para desviodados
38 - li $r9 1901
39 - sw $r9 0($r6) # movendo t2 para desvio
40 - L0: #Nova Label L0
41 - lw $r8 0($r5) # movendo i para t3
42 - li $r10 6
43 - addil $r43 $r44 L1
44 - ble $r8 $r10 L1 # jump se <=
45 - addil $r43 $r44 L2
46 - j L2
47 - L1: #Nova Label L1
48 - nop 0
49 - lw $r11 0($r5) # movendo i para t6
50 - lw $r12 0($r7) # movendo desviodados para t7
51 - sub $r13 $r37 $r11    # endereço base - deslocamento
52 - sw $r12 0($r13)      # armazena t7 em dados[t6]
53 - lw $r12 0($r5) # movendo i para t8
54 - li $r13 0
55 - sub $r10 $r34 $r12    # endereço base - deslocamento
56 - sw $r13 0($r10)      # armazena t9 em halts[t8]
57 - lw $r10 0($r5) # movendo i para t10
58 - li $r14 0
59 - sub $r13 $r38 $r10    # endereço base - deslocamento
60 - sw $r14 0($r13)      # armazena t11 em rodouumavez[t10]
61 - lw $r13 0($r5) # movendo i para t12
62 - lw $r14 0($r5) # movendo i para t13
63 - sub $r15 $r33 $r13    # endereço base - deslocamento
64 - sw $r14 0($r15)      # armazena t13 em programas[t12]
65 - lw $r14 0($r5) # movendo i para t14
66 - lw $r15 0($r6) # movendo desvio para t15
67 - sub $r16 $r61 $r14    # endereço base - deslocamento
68 - sw $r15 0($r16)      # armazena t15 em processos[t14]
69 - lw $r15 0($r6) # movendo desvio para t16
70 - li $r17 300
71 - add $r16 $r15 $r17 # salva em t18 (r16) 
72 - sw $r16 0($r6) # movendo t18 para desvio
73 - lw $r15 0($r7) # movendo desviodados para t19
74 - li $r17 1000
75 - sub $r16 $r15 $r17 # salva em t21 (r16) 
76 - sw $r16 0($r7) # movendo t21 para desviodados
77 - lw $r15 0($r5) # movendo i para t22
78 - li $r17 1
79 - add $r16 $r15 $r17 # salva em t24 (r16) 
80 - sw $r16 0($r5) # movendo t24 para i
81 - nop 0
82 - addil $r43 $r44 L0
83 - j L0
84 - L2: #Nova Label L2
85 - move $r1 $r2         # restaura stack pointer
86 - lw $r2 0($r1)       # desempilha registrador
87 - addi $r1 $r1 1       #desaloca espaço na pilha
88 - lw $r31 0($r1)       # desempilha registrador
89 - addi $r1 $r1 1       #desaloca espaço na pilha
90 - move $r0 $r0
91 - jr $r31         # retorna (void function end)
92 - limpaVar: # nova função limpaVar
93 - subi $r1 $r1 1      # aloca espaço na pilha
94 - sw $r31 0($r1)       # empilha registrador
95 - subi $r1 $r1 1      # aloca espaço na pilha
96 - sw $r2 0($r1)       # empilha registrador
97 - move $r2 $r1         # fp = sp
98 - subi $r1 $r1 1 # aloca espaço para variável 'i'
99 - sw $r63 0($r1)  # inicializa com 0
100 - move $r5 $r1   # endereço da variável 'i'
101 - li $r7 0
102 - sw $r7 0($r5) # movendo t0 para i
103 - L3: #Nova Label L3
104 - lw $r6 0($r5) # movendo i para t1
105 - li $r8 9
106 - addil $r43 $r44 L4
107 - ble $r6 $r8 L4 # jump se <=
108 - addil $r43 $r44 L5
109 - j L5
110 - L4: #Nova Label L4
111 - nop 0
112 - lw $r9 0($r5) # movendo i para t4
113 - li $r10 0
114 - sub $r8 $r34 $r9    # endereço base - deslocamento
115 - sw $r10 0($r8)      # armazena t5 em halts[t4]
116 - lw $r8 0($r5) # movendo i para t6
117 - li $r11 0
118 - sub $r10 $r38 $r8    # endereço base - deslocamento
119 - sw $r11 0($r10)      # armazena t7 em rodouumavez[t6]
120 - lw $r10 0($r5) # movendo i para t8
121 - li $r12 0
122 - sub $r11 $r36 $r10    # endereço base - deslocamento
123 - sw $r12 0($r11)      # armazena t9 em processosExecucao[t8]
124 - lw $r11 0($r5) # movendo i para t10
125 - li $r13 1
126 - add $r12 $r11 $r13 # salva em t12 (r12) 
127 - sw $r12 0($r5) # movendo t12 para i
128 - nop 0
129 - addil $r43 $r44 L3
130 - j L3
131 - L5: #Nova Label L5
132 - li $r8 0
133 - sw $r8 0($r35) # movendo t13 para qtdHalt
134 - li $r11 0
135 - move $r51 $r11 # movendo t14 para registrador especial sinalsyscall
136 - li $r12 0
137 - move $r41 $r12 # movendo t15 para registrador especial opcao
138 - li $r13 0
139 - move $r60 $r13 # movendo t16 para registrador especial processosCarregados
140 - move $r1 $r2         # restaura stack pointer
141 - lw $r2 0($r1)       # desempilha registrador
142 - addi $r1 $r1 1       #desaloca espaço na pilha
143 - lw $r31 0($r1)       # desempilha registrador
144 - addi $r1 $r1 1       #desaloca espaço na pilha
145 - move $r0 $r0
146 - jr $r31         # retorna (void function end)
147 - menuShell: # nova função menuShell
148 - subi $r1 $r1 1      # aloca espaço na pilha
149 - sw $r31 0($r1)       # empilha registrador
150 - subi $r1 $r1 1      # aloca espaço na pilha
151 - sw $r2 0($r1)       # empilha registrador
152 - move $r2 $r1         # fp = sp
153 - li $r46 26 # argument 0 (26)
154 - msgLcd $r46
155 - in $r3
156 - move $r7 $r3 # move valor de input para t1
157 - move $r41 $r7 # movendo t1 para registrador especial opcao
158 - move $r1 $r2         # restaura stack pointer
159 - lw $r2 0($r1)       # desempilha registrador
160 - addi $r1 $r1 1       #desaloca espaço na pilha
161 - lw $r31 0($r1)       # desempilha registrador
162 - addi $r1 $r1 1       #desaloca espaço na pilha
163 - move $r0 $r0
164 - jr $r31         # retorna (void function end)
165 - votenaoPreemptivo: # nova função votenaoPreemptivo
166 - subi $r1 $r1 1      # aloca espaço na pilha
167 - sw $r31 0($r1)       # empilha registrador
168 - subi $r1 $r1 1      # aloca espaço na pilha
169 - sw $r2 0($r1)       # empilha registrador
170 - move $r2 $r1         # fp = sp
171 - li $r5 6
172 - move $r59 $r5 # movendo t0 para registrador especial processoAtual
173 - nop 0
174 - li $r46 31 # argument 0 (31)
175 - msgLcd $r46
176 - move $r8 $r59 # movendo processoAtual para registrador especial t2
177 - sub $r10 $r61 $r8    # endereço base - deslocamento
178 - lw $r9 0($r10)      # carrega processos[t2] em t3
179 - move $r44 $r9 # movendo t3 para registrador especial salto
180 - move $r10 $r59 # movendo processoAtual para registrador especial t4
181 - sub $r12 $r37 $r10    # endereço base - deslocamento
182 - lw $r11 0($r12)      # carrega dados[t4] em t5
183 - move $r62 $r11 # movendo t5 para registrador especial memdados
184 - move $r42 $r1 # salva a posição de memoria
185 - move $r1 $r40
186 - li $r58 179 
187 - li $r43 1001
188 - j 1001
189 - move $r1 $r40 # carrega o valor salvo
190 - li $r58 183 
191 - li $r43 1201
192 - j 1201
193 - move $r1 $r42 # carrega a posição de memoria
194 - nop 0
195 - li $r12 0
196 - move $r44 $r12 # movendo t6 para registrador especial salto
197 - nop 0
198 - move $r1 $r2         # restaura stack pointer
199 - lw $r2 0($r1)       # desempilha registrador
200 - addi $r1 $r1 1       #desaloca espaço na pilha
201 - lw $r31 0($r1)       # desempilha registrador
202 - addi $r1 $r1 1       #desaloca espaço na pilha
203 - move $r0 $r0
204 - jr $r31         # retorna (void function end)
205 - execucaoNP: # nova função execucaoNP
206 - subi $r1 $r1 1      # aloca espaço na pilha
207 - sw $r31 0($r1)       # empilha registrador
208 - subi $r1 $r1 1      # aloca espaço na pilha
209 - sw $r2 0($r1)       # empilha registrador
210 - move $r2 $r1         # fp = sp
211 - subi $r1 $r1 1 # aloca espaço para variável 'i'
212 - sw $r63 0($r1)  # inicializa com 0
213 - move $r5 $r1   # endereço da variável 'i'
214 - li $r7 0
215 - sw $r7 0($r5) # movendo t0 para i
216 - L6: #Nova Label L6
217 - lw $r6 0($r5) # movendo i para t1
218 - li $r8 3
219 - addil $r43 $r44 L7
220 - blt $r6 $r8 L7 #jump se <
221 - addil $r43 $r44 L8
222 - j L8
223 - L7: #Nova Label L7
224 - lw $r8 0($r5) # movendo i para t4
225 - li $r10 1
226 - add $r9 $r8 $r10 # salva em t6 (r9) 
227 - sub $r10 $r36 $r9    # endereço base - deslocamento
228 - lw $r8 0($r10)      # carrega processosExecucao[t6] em t7
229 - move $r59 $r8 # movendo t7 para registrador especial processoAtual
230 - nop 0
231 - li $r46 31 # argument 0 (31)
232 - msgLcd $r46
233 - move $r13 $r59 # movendo processoAtual para registrador especial t9
234 - sub $r15 $r61 $r13    # endereço base - deslocamento
235 - lw $r14 0($r15)      # carrega processos[t9] em t10
236 - move $r44 $r14 # movendo t10 para registrador especial salto
237 - move $r15 $r59 # movendo processoAtual para registrador especial t11
238 - sub $r17 $r37 $r15    # endereço base - deslocamento
239 - lw $r16 0($r17)      # carrega dados[t11] em t12
240 - move $r62 $r16 # movendo t12 para registrador especial memdados
241 - move $r42 $r1 # salva a posição de memoria
242 - move $r1 $r40
243 - li $r58 233 
244 - li $r43 1001
245 - j 1001
246 - move $r1 $r40 # carrega o valor salvo
247 - li $r58 237 
248 - li $r43 1201
249 - j 1201
250 - move $r1 $r42 # carrega a posição de memoria
251 - nop 0
252 - li $r17 0
253 - move $r44 $r17 # movendo t13 para registrador especial salto
254 - nop 0
255 - lw $r46 0($r5)    # carrega valor da variável local 'i' para argument 0
256 - move $r0 $r46
257 - out $r0
258 - lw $r18 0($r5) # movendo i para t15
259 - li $r19 1
260 - add $r12 $r18 $r19 # salva em t17 (r12) 
261 - sw $r12 0($r5) # movendo t17 para i
262 - addil $r43 $r44 L6
263 - j L6
264 - L8: #Nova Label L8
265 - nop 0
266 - li $r8 0
267 - move $r44 $r8 # movendo t18 para registrador especial salto
268 - nop 0
269 - move $r1 $r2         # restaura stack pointer
270 - lw $r2 0($r1)       # desempilha registrador
271 - addi $r1 $r1 1       #desaloca espaço na pilha
272 - lw $r31 0($r1)       # desempilha registrador
273 - addi $r1 $r1 1       #desaloca espaço na pilha
274 - move $r0 $r0
275 - jr $r31         # retorna (void function end)
276 - defineProcesso: # nova função defineProcesso
277 - subi $r1 $r1 1      # aloca espaço na pilha
278 - sw $r31 0($r1)       # empilha registrador
279 - subi $r1 $r1 1      # aloca espaço na pilha
280 - sw $r2 0($r1)       # empilha registrador
281 - move $r2 $r1         # fp = sp
282 - li $r46 27 # argument 0 (27)
283 - msgLcd $r46
284 - nop 0
285 - nop 0
286 - move $r7 $r41 # movendo opcao para registrador especial t1
287 - li $r8 1
288 - addil $r43 $r44 L9
289 - bnq $r7 $r8 L9 # jump se t1 != t2
290 - li $r10 1
291 - li $r9 0
292 - sub $r5 $r36 $r10    # endereço base - deslocamento
293 - sw $r9 0($r5)      # armazena t5 em processosExecucao[t4]
294 - li $r9 2
295 - li $r11 1
296 - sub $r5 $r36 $r9    # endereço base - deslocamento
297 - sw $r11 0($r5)      # armazena t7 em processosExecucao[t6]
298 - li $r11 3
299 - li $r12 2
300 - sub $r5 $r36 $r11    # endereço base - deslocamento
301 - sw $r12 0($r5)      # armazena t9 em processosExecucao[t8]
302 - addil $r43 $r44 L10
303 - j L10
304 - L9: #Nova Label L9
305 - move $r5 $r41 # movendo opcao para registrador especial t10
306 - li $r12 2
307 - addil $r43 $r44 L11
308 - bnq $r5 $r12 L11 # jump se t10 != t11
309 - li $r14 1
310 - li $r15 0
311 - sub $r13 $r36 $r14    # endereço base - deslocamento
312 - sw $r15 0($r13)      # armazena t14 em processosExecucao[t13]
313 - li $r15 2
314 - li $r16 1
315 - sub $r13 $r36 $r15    # endereço base - deslocamento
316 - sw $r16 0($r13)      # armazena t16 em processosExecucao[t15]
317 - li $r16 3
318 - li $r17 3
319 - sub $r13 $r36 $r16    # endereço base - deslocamento
320 - sw $r17 0($r13)      # armazena t18 em processosExecucao[t17]
321 - addil $r43 $r44 L12
322 - j L12
323 - L11: #Nova Label L11
324 - move $r12 $r41 # movendo opcao para registrador especial t19
325 - li $r17 3
326 - addil $r43 $r44 L13
327 - bnq $r12 $r17 L13 # jump se t19 != t20
328 - li $r19 1
329 - li $r20 0
330 - sub $r18 $r36 $r19    # endereço base - deslocamento
331 - sw $r20 0($r18)      # armazena t23 em processosExecucao[t22]
332 - li $r20 2
333 - li $r21 3
334 - sub $r18 $r36 $r20    # endereço base - deslocamento
335 - sw $r21 0($r18)      # armazena t25 em processosExecucao[t24]
336 - li $r21 3
337 - li $r22 4
338 - sub $r18 $r36 $r21    # endereço base - deslocamento
339 - sw $r22 0($r18)      # armazena t27 em processosExecucao[t26]
340 - addil $r43 $r44 L14
341 - j L14
342 - L13: #Nova Label L13
343 - move $r17 $r41 # movendo opcao para registrador especial t28
344 - li $r22 4
345 - addil $r43 $r44 L15
346 - bnq $r17 $r22 L15 # jump se t28 != t29
347 - li $r24 1
348 - li $r25 3
349 - sub $r23 $r36 $r24    # endereço base - deslocamento
350 - sw $r25 0($r23)      # armazena t32 em processosExecucao[t31]
351 - li $r25 2
352 - li $r26 4
353 - sub $r23 $r36 $r25    # endereço base - deslocamento
354 - sw $r26 0($r23)      # armazena t34 em processosExecucao[t33]
355 - li $r26 3
356 - li $r27 5
357 - sub $r23 $r36 $r26    # endereço base - deslocamento
358 - sw $r27 0($r23)      # armazena t36 em processosExecucao[t35]
359 - addil $r43 $r44 L16
360 - j L16
361 - L15: #Nova Label L15
362 - nop 0
363 - L16: #Nova Label L16
364 - L14: #Nova Label L14
365 - L12: #Nova Label L12
366 - L10: #Nova Label L10
367 - move $r1 $r2         # restaura stack pointer
368 - lw $r2 0($r1)       # desempilha registrador
369 - addi $r1 $r1 1       #desaloca espaço na pilha
370 - lw $r31 0($r1)       # desempilha registrador
371 - addi $r1 $r1 1       #desaloca espaço na pilha
372 - move $r0 $r0
373 - jr $r31         # retorna (void function end)
374 - initRR: # nova função initRR
375 - subi $r1 $r1 1      # aloca espaço na pilha
376 - sw $r31 0($r1)       # empilha registrador
377 - subi $r1 $r1 1      # aloca espaço na pilha
378 - sw $r2 0($r1)       # empilha registrador
379 - move $r2 $r1         # fp = sp
380 - nop 0
381 - li $r5 0
382 - sw $r5 0($r35) # movendo t0 para qtdHalt
383 - nop 0
384 - addil $r43 $r44 execucaoNP
385 - jal execucaoNP
386 - nop 0
387 - li $r46 29 # argument 0 (29)
388 - msgLcd $r46
389 - nop 0
390 - move $r1 $r2         # restaura stack pointer
391 - lw $r2 0($r1)       # desempilha registrador
392 - addi $r1 $r1 1       #desaloca espaço na pilha
393 - lw $r31 0($r1)       # desempilha registrador
394 - addi $r1 $r1 1       #desaloca espaço na pilha
395 - move $r0 $r0
396 - jr $r31         # retorna (void function end)
397 - initDados: # nova função initDados
398 - subi $r1 $r1 1      # aloca espaço na pilha
399 - sw $r31 0($r1)       # empilha registrador
400 - subi $r1 $r1 1      # aloca espaço na pilha
401 - sw $r2 0($r1)       # empilha registrador
402 - move $r2 $r1         # fp = sp
403 - subi $r1 $r1 1 # aloca espaço para variável 'temp'
404 - sw $r63 0($r1)  # inicializa com 0
405 - move $r5 $r1   # endereço da variável 'temp'
406 - subi $r1 $r1 1 # aloca espaço para variável 'combustivel'
407 - sw $r63 0($r1)  # inicializa com 0
408 - move $r6 $r1   # endereço da variável 'combustivel'
409 - subi $r1 $r1 1 # aloca espaço para variável 'altura'
410 - sw $r63 0($r1)  # inicializa com 0
411 - move $r7 $r1   # endereço da variável 'altura'
412 - li $r9 25
413 - sw $r9 0($r5) # movendo t0 para temp
414 - li $r9 255
415 - sw $r9 0($r6) # movendo t1 para combustivel
416 - li $r9 0
417 - sw $r9 0($r7) # movendo t2 para altura
418 - li $r46 2000 # argument 0 (2000)
419 - lw $r47 0($r5)    # carrega valor da variável local 'temp' para argument 1
420 - sw $r47 0($r46)
421 - li $r46 2001 # argument 0 (2001)
422 - lw $r47 0($r6)    # carrega valor da variável local 'combustivel' para argument 1
423 - sw $r47 0($r46)
424 - li $r46 2002 # argument 0 (2002)
425 - lw $r47 0($r7)    # carrega valor da variável local 'altura' para argument 1
426 - sw $r47 0($r46)
427 - move $r1 $r2         # restaura stack pointer
428 - lw $r2 0($r1)       # desempilha registrador
429 - addi $r1 $r1 1       #desaloca espaço na pilha
430 - lw $r31 0($r1)       # desempilha registrador
431 - addi $r1 $r1 1       #desaloca espaço na pilha
432 - move $r0 $r0
433 - jr $r31         # retorna (void function end)
434 - initUART: # nova função initUART
435 - subi $r1 $r1 1      # aloca espaço na pilha
436 - sw $r31 0($r1)       # empilha registrador
437 - subi $r1 $r1 1      # aloca espaço na pilha
438 - sw $r2 0($r1)       # empilha registrador
439 - move $r2 $r1         # fp = sp
440 - li $r46 32 # argument 0 (32)
441 - msgLcd $r46
442 - nop 0
443 - move $r1 $r2         # restaura stack pointer
444 - lw $r2 0($r1)       # desempilha registrador
445 - addi $r1 $r1 1       #desaloca espaço na pilha
446 - lw $r31 0($r1)       # desempilha registrador
447 - addi $r1 $r1 1       #desaloca espaço na pilha
448 - move $r0 $r0
449 - jr $r31         # retorna (void function end)
450 - receiveUART: # nova função receiveUART
451 - subi $r1 $r1 1      # aloca espaço na pilha
452 - sw $r31 0($r1)       # empilha registrador
453 - subi $r1 $r1 1      # aloca espaço na pilha
454 - sw $r2 0($r1)       # empilha registrador
455 - move $r2 $r1         # fp = sp
456 - subi $r1 $r1 1 # aloca espaço para variável 'pronto'
457 - sw $r63 0($r1)  # inicializa com 0
458 - move $r5 $r1   # endereço da variável 'pronto'
459 - subi $r1 $r1 1 # aloca espaço para variável 'aux'
460 - sw $r63 0($r1)  # inicializa com 0
461 - move $r6 $r1   # endereço da variável 'aux'
462 - subi $r1 $r1 1 # aloca espaço para variável 'header'
463 - sw $r63 0($r1)  # inicializa com 0
464 - move $r7 $r1   # endereço da variável 'header'
465 - subi $r1 $r1 1 # aloca espaço para variável 'flag'
466 - sw $r63 0($r1)  # inicializa com 0
467 - move $r8 $r1   # endereço da variável 'flag'
468 - subi $r1 $r1 1 # aloca espaço para variável 'comando'
469 - sw $r63 0($r1)  # inicializa com 0
470 - move $r9 $r1   # endereço da variável 'comando'
471 - subi $r1 $r1 1 # aloca espaço para variável 'valor'
472 - sw $r63 0($r1)  # inicializa com 0
473 - move $r10 $r1   # endereço da variável 'valor'
474 - subi $r1 $r1 1 # aloca espaço para variável 'checksum'
475 - sw $r63 0($r1)  # inicializa com 0
476 - move $r11 $r1   # endereço da variável 'checksum'
477 - li $r46 33 # argument 0 (33)
478 - msgLcd $r46
479 - li $r16 1000
480 - sw $r16 0($r6) # movendo t1 para aux
481 - li $r15 0
482 - sw $r15 0($r5) # movendo t2 para pronto
483 - L17: #Nova Label L17
484 - lw $r13 0($r5) # movendo pronto para t3
485 - li $r16 0
486 - addil $r43 $r44 L18
487 - beq $r13 $r16 L18 # jump se t3 == t4
488 - addil $r43 $r44 L19
489 - j L19
490 - L18: #Nova Label L18
491 - lw $r46 0($r6)    # carrega valor da variável local 'aux' para argument 0
492 - lw $r17 0($r46)
493 - sw $r17 0($r5) # movendo t7 para pronto
494 - addil $r43 $r44 L17
495 - j L17
496 - L19: #Nova Label L19
497 - lw $r16 0($r6) # movendo aux para t8
498 - li $r17 1
499 - add $r14 $r16 $r17 # salva em t10 (r14) 
500 - move $r46 $r14 # argument 0 (t10)
501 - li $r47 1 # argument 1 (1)
502 - sw $r47 0($r46)
503 - lw $r19 0($r6) # movendo aux para t12
504 - li $r20 2
505 - add $r18 $r19 $r20 # salva em t14 (r18) 
506 - move $r46 $r18 # argument 0 (t14)
507 - lw $r19 0($r46)
508 - sw $r19 0($r7) # movendo t15 para header
509 - lw $r46 0($r7)    # carrega valor da variável local 'header' para argument 0
510 - move $r0 $r46
511 - out $r0
512 - lw $r19 0($r6) # movendo aux para t17
513 - li $r21 3
514 - add $r20 $r19 $r21 # salva em t19 (r20) 
515 - move $r46 $r20 # argument 0 (t19)
516 - lw $r19 0($r46)
517 - sw $r19 0($r8) # movendo t20 para flag
518 - lw $r46 0($r8)    # carrega valor da variável local 'flag' para argument 0
519 - move $r0 $r46
520 - out $r0
521 - lw $r19 0($r6) # movendo aux para t22
522 - li $r21 4
523 - add $r4 $r19 $r21 # salva em t24 (r4) 
524 - move $r46 $r4 # argument 0 (t24)
525 - lw $r19 0($r46)
526 - sw $r19 0($r9) # movendo t25 para comando
527 - lw $r46 0($r9)    # carrega valor da variável local 'comando' para argument 0
528 - move $r0 $r46
529 - out $r0
530 - lw $r19 0($r6) # movendo aux para t27
531 - li $r22 5
532 - add $r21 $r19 $r22 # salva em t29 (r21) 
533 - move $r46 $r21 # argument 0 (t29)
534 - lw $r19 0($r46)
535 - sw $r19 0($r10) # movendo t30 para valor
536 - lw $r46 0($r10)    # carrega valor da variável local 'valor' para argument 0
537 - move $r0 $r46
538 - out $r0
539 - lw $r19 0($r6) # movendo aux para t32
540 - li $r23 6
541 - add $r22 $r19 $r23 # salva em t34 (r22) 
542 - move $r46 $r22 # argument 0 (t34)
543 - lw $r19 0($r46)
544 - sw $r19 0($r11) # movendo t35 para checksum
545 - lw $r46 0($r11)    # carrega valor da variável local 'checksum' para argument 0
546 - move $r0 $r46
547 - out $r0
548 - li $r46 500 # argument 0 (500)
549 - lw $r47 0($r7)    # carrega valor da variável local 'header' para argument 1
550 - sw $r47 0($r46)
551 - li $r46 501 # argument 0 (501)
552 - lw $r47 0($r8)    # carrega valor da variável local 'flag' para argument 1
553 - sw $r47 0($r46)
554 - li $r46 502 # argument 0 (502)
555 - lw $r47 0($r9)    # carrega valor da variável local 'comando' para argument 1
556 - sw $r47 0($r46)
557 - li $r46 503 # argument 0 (503)
558 - lw $r47 0($r10)    # carrega valor da variável local 'valor' para argument 1
559 - sw $r47 0($r46)
560 - li $r46 504 # argument 0 (504)
561 - lw $r47 0($r11)    # carrega valor da variável local 'checksum' para argument 1
562 - sw $r47 0($r46)
563 - nop 0
564 - move $r1 $r2         # restaura stack pointer
565 - lw $r2 0($r1)       # desempilha registrador
566 - addi $r1 $r1 1       #desaloca espaço na pilha
567 - lw $r31 0($r1)       # desempilha registrador
568 - addi $r1 $r1 1       #desaloca espaço na pilha
569 - move $r0 $r0
570 - jr $r31         # retorna (void function end)
571 - checkFlags: # nova função checkFlags
572 - subi $r1 $r1 1      # aloca espaço na pilha
573 - sw $r31 0($r1)       # empilha registrador
574 - subi $r1 $r1 1      # aloca espaço na pilha
575 - sw $r2 0($r1)       # empilha registrador
576 - move $r2 $r1         # fp = sp
577 - lw $r52 2($r2)      # carrega param 0
578 - subi $r1 $r1 1 # aloca espaço para variável 'valorAns'
579 - sw $r63 0($r1)  # inicializa com 0
580 - move $r5 $r1   # endereço da variável 'valorAns'
581 - move $r6 $r52 # movendo flag para t0
582 - li $r8 1
583 - addil $r43 $r44 L20
584 - bnq $r6 $r8 L20 # jump se t0 != t1
585 - li $r46 2000 # argument 0 (2000)
586 - lw $r12 0($r46)
587 - sw $r12 0($r5) # movendo t4 para valorAns
588 - addil $r43 $r44 L21
589 - j L21
590 - L20: #Nova Label L20
591 - move $r8 $r52 # movendo flag para t5
592 - li $r13 2
593 - addil $r43 $r44 L22
594 - bnq $r8 $r13 L22 # jump se t5 != t6
595 - li $r46 2001 # argument 0 (2001)
596 - lw $r15 0($r46)
597 - sw $r15 0($r5) # movendo t9 para valorAns
598 - addil $r43 $r44 L23
599 - j L23
600 - L22: #Nova Label L22
601 - move $r13 $r52 # movendo flag para t10
602 - li $r16 3
603 - addil $r43 $r44 L24
604 - bnq $r13 $r16 L24 # jump se t10 != t11
605 - li $r46 2002 # argument 0 (2002)
606 - lw $r18 0($r46)
607 - sw $r18 0($r5) # movendo t14 para valorAns
608 - addil $r43 $r44 L25
609 - j L25
610 - L24: #Nova Label L24
611 - li $r16 0
612 - sw $r16 0($r5) # movendo t15 para valorAns
613 - nop 0
614 - L25: #Nova Label L25
615 - L23: #Nova Label L23
616 - L21: #Nova Label L21
617 - lw $r16 0($r5) # movendo valorAns para t16
618 - move $r45 $r16 # move valor de retorno para v0
619 - move $r1 $r2         # restaura stack pointer
620 - lw $r2 0($r1)       # desempilha registrador
621 - addi $r1 $r1 1       #desaloca espaço na pilha
622 - lw $r31 0($r1)       # desempilha registrador
623 - addi $r1 $r1 1       #desaloca espaço na pilha
624 - jr $r31         # retorna
625 - sendUART: # nova função sendUART
626 - subi $r1 $r1 1      # aloca espaço na pilha
627 - sw $r31 0($r1)       # empilha registrador
628 - subi $r1 $r1 1      # aloca espaço na pilha
629 - sw $r2 0($r1)       # empilha registrador
630 - move $r2 $r1         # fp = sp
631 - subi $r1 $r1 1 # aloca espaço para variável 'comando'
632 - sw $r63 0($r1)  # inicializa com 0
633 - move $r5 $r1   # endereço da variável 'comando'
634 - subi $r1 $r1 1 # aloca espaço para variável 'flag'
635 - sw $r63 0($r1)  # inicializa com 0
636 - move $r6 $r1   # endereço da variável 'flag'
637 - subi $r1 $r1 1 # aloca espaço para variável 'valorRec'
638 - sw $r63 0($r1)  # inicializa com 0
639 - move $r7 $r1   # endereço da variável 'valorRec'
640 - subi $r1 $r1 1 # aloca espaço para variável 'valorAns'
641 - sw $r63 0($r1)  # inicializa com 0
642 - move $r8 $r1   # endereço da variável 'valorAns'
643 - subi $r1 $r1 1 # aloca espaço para variável 'header'
644 - sw $r63 0($r1)  # inicializa com 0
645 - move $r9 $r1   # endereço da variável 'header'
646 - subi $r1 $r1 1 # aloca espaço para variável 'checksum'
647 - sw $r63 0($r1)  # inicializa com 0
648 - move $r10 $r1   # endereço da variável 'checksum'
649 - li $r46 34 # argument 0 (34)
650 - msgLcd $r46
651 - li $r46 501 # argument 0 (501)
652 - lw $r15 0($r46)
653 - sw $r15 0($r6) # movendo t2 para flag
654 - li $r46 502 # argument 0 (502)
655 - lw $r16 0($r46)
656 - sw $r16 0($r5) # movendo t4 para comando
657 - li $r46 503 # argument 0 (503)
658 - lw $r17 0($r46)
659 - sw $r17 0($r7) # movendo t6 para valorRec
660 - lw $r17 0($r5) # movendo comando para t7
661 - li $r18 1
662 - addil $r43 $r44 L26
663 - bnq $r17 $r18 L26 # jump se t7 != t8
664 - li $r20 124
665 - sw $r20 0($r9) # movendo t10 para header
666 - lw $r19 0($r7) # movendo valorRec para t11
667 - sw $r19 0($r8) # movendo t11 para valorAns
668 - addil $r43 $r44 L27
669 - j L27
670 - L26: #Nova Label L26
671 - lw $r18 0($r5) # movendo comando para t12
672 - li $r20 2
673 - addil $r43 $r44 L28
674 - bnq $r18 $r20 L28 # jump se t12 != t13
675 - li $r22 124
676 - sw $r22 0($r9) # movendo t15 para header
677 - lw $r21 0($r7) # movendo valorRec para t16
678 - sw $r21 0($r8) # movendo t16 para valorAns
679 - addil $r43 $r44 L29
680 - j L29
681 - L28: #Nova Label L28
682 - lw $r20 0($r5) # movendo comando para t17
683 - li $r22 3
684 - addil $r43 $r44 L30
685 - bnq $r20 $r22 L30 # jump se t17 != t18
686 - li $r24 124
687 - lw $r23 0($r6) # movendo flag para t21
688 - add $r25 $r24 $r23 # salva em t22 (r25) 
689 - sw $r25 0($r9) # movendo t22 para header
690 - subi $r1 $r1 1  # aloca espaço para 1 argumentos
691 - lw $r46 0($r6)    # carrega valor da variável local 'flag' para argument 0
692 - sw $r46 0($r1)  # salva argument 0 na pilha
693 - addil $r43 $r44 checkFlags
694 - jal checkFlags
695 - addi $r1 $r1 1  # libera espaço de 1 argumentos
696 - lw $r5 -1($r2)  # recarrega variavel local
697 - lw $r6 -2($r2)  # recarrega variavel local
698 - lw $r7 -3($r2)  # recarrega variavel local
699 - lw $r8 -4($r2)  # recarrega variavel local
700 - lw $r9 -5($r2)  # recarrega variavel local
701 - lw $r10 -6($r2)  # recarrega variavel local
702 - move $r24 $r45 # copia retorno (v0) para t24
703 - sw $r24 0($r8) # movendo t24 para valorAns
704 - addil $r43 $r44 L31
705 - j L31
706 - L30: #Nova Label L30
707 - nop 0
708 - L31: #Nova Label L31
709 - L29: #Nova Label L29
710 - L27: #Nova Label L27
711 - li $r46 1008 # argument 0 (1008)
712 - lw $r47 0($r9)    # carrega valor da variável local 'header' para argument 1
713 - sw $r47 0($r46)
714 - li $r46 1009 # argument 0 (1009)
715 - lw $r47 0($r5)    # carrega valor da variável local 'comando' para argument 1
716 - sw $r47 0($r46)
717 - li $r46 1010 # argument 0 (1010)
718 - lw $r47 0($r8)    # carrega valor da variável local 'valorAns' para argument 1
719 - sw $r47 0($r46)
720 - li $r46 1011 # argument 0 (1011)
721 - li $r47 1 # argument 1 (1)
722 - sw $r47 0($r46)
723 - nop 0
724 - move $r1 $r2         # restaura stack pointer
725 - lw $r2 0($r1)       # desempilha registrador
726 - addi $r1 $r1 1       #desaloca espaço na pilha
727 - lw $r31 0($r1)       # desempilha registrador
728 - addi $r1 $r1 1       #desaloca espaço na pilha
729 - move $r0 $r0
730 - jr $r31         # retorna (void function end)
731 - main: # nova função main
732 - addil $r43 $r44 mapeamento
733 - jal mapeamento
734 - li $r46 0 # argument 0 (0)
735 - msgLcd $r46
736 - nop 0
737 - li $r6 0
738 - move $r44 $r6 # movendo t1 para registrador especial salto
739 - li $r7 3
740 - move $r60 $r7 # movendo t2 para registrador especial processosCarregados
741 - nop 0
742 - nop 0
743 - addil $r43 $r44 menuShell
744 - jal menuShell
745 - nop 0
746 - li $r8 0
747 - move $r44 $r8 # movendo t3 para registrador especial salto
748 - nop 0
749 - nop 0
750 - addil $r43 $r44 defineProcesso
751 - jal defineProcesso
752 - addil $r43 $r44 initDados
753 - jal initDados
754 - addil $r43 $r44 initUART
755 - jal initUART
756 - L32: #Nova Label L32
757 - move $r9 $r41 # movendo opcao para registrador especial t4
758 - li $r11 6
759 - addil $r43 $r44 L33
760 - bnq $r9 $r11 L33 # jump se !=
761 - addil $r43 $r44 L34
762 - j L34
763 - L33: #Nova Label L33
764 - addil $r43 $r44 receiveUART
765 - jal receiveUART
766 - addil $r43 $r44 initRR
767 - jal initRR
768 - addil $r43 $r44 votenaoPreemptivo
769 - jal votenaoPreemptivo
770 - addil $r43 $r44 sendUART
771 - jal sendUART
772 - nop 0
773 - li $r11 0
774 - move $r44 $r11 # movendo t7 para registrador especial salto
775 - addil $r43 $r44 menuShell
776 - jal menuShell
777 - addil $r43 $r44 L32
778 - j L32
779 - L34: #Nova Label L34
780 - nop 0
781 - nop 0
782 - nop 0
783 - li $r46 28 # argument 0 (28)
784 - msgLcd $r46
785 - nop 0
786 - nop 0
787 - nop 0
788 - nop 0
789 - halt # termina a execução
