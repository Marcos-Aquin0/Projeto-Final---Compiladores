0 - move $r32 $r1 # endereço bcp
1 - subi $r1 $r1 70 # endereço bcp
2 - move $r61 $r1   # endereço base do array 'processos'
3 - subi $r1 $r1 10  # próximo elemento
4 - move $r33 $r1   # endereço base do array 'programas'
5 - subi $r1 $r1 10  # próximo elemento
6 - move $r34 $r1   # endereço base do array 'halts'
7 - subi $r1 $r1 10  # próximo elemento
8 - subi $r1 $r1 1 # aloca espaço para variável 'comandoUart'
9 - sw $r63 0($r1)  # inicializa com 0
10 - move $r35 $r1   # endereço da variável 'comandoUart'
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
133 - sw $r8 0($r35) # movendo t13 para comandoUart
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
231 - li $r46 12 # argument 0 (12)
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
374 - initDados: # nova função initDados
375 - subi $r1 $r1 1      # aloca espaço na pilha
376 - sw $r31 0($r1)       # empilha registrador
377 - subi $r1 $r1 1      # aloca espaço na pilha
378 - sw $r2 0($r1)       # empilha registrador
379 - move $r2 $r1         # fp = sp
380 - subi $r1 $r1 1 # aloca espaço para variável 'temp'
381 - sw $r63 0($r1)  # inicializa com 0
382 - move $r5 $r1   # endereço da variável 'temp'
383 - subi $r1 $r1 1 # aloca espaço para variável 'combustivel'
384 - sw $r63 0($r1)  # inicializa com 0
385 - move $r6 $r1   # endereço da variável 'combustivel'
386 - subi $r1 $r1 1 # aloca espaço para variável 'altura'
387 - sw $r63 0($r1)  # inicializa com 0
388 - move $r7 $r1   # endereço da variável 'altura'
389 - li $r9 25
390 - sw $r9 0($r5) # movendo t0 para temp
391 - li $r9 255
392 - sw $r9 0($r6) # movendo t1 para combustivel
393 - li $r9 0
394 - sw $r9 0($r7) # movendo t2 para altura
395 - li $r46 2000 # argument 0 (2000)
396 - lw $r47 0($r5)    # carrega valor da variável local 'temp' para argument 1
397 - sw $r47 0($r46)
398 - li $r46 2001 # argument 0 (2001)
399 - lw $r47 0($r6)    # carrega valor da variável local 'combustivel' para argument 1
400 - sw $r47 0($r46)
401 - li $r46 2002 # argument 0 (2002)
402 - lw $r47 0($r7)    # carrega valor da variável local 'altura' para argument 1
403 - sw $r47 0($r46)
404 - move $r1 $r2         # restaura stack pointer
405 - lw $r2 0($r1)       # desempilha registrador
406 - addi $r1 $r1 1       #desaloca espaço na pilha
407 - lw $r31 0($r1)       # desempilha registrador
408 - addi $r1 $r1 1       #desaloca espaço na pilha
409 - move $r0 $r0
410 - jr $r31         # retorna (void function end)
411 - initUART: # nova função initUART
412 - subi $r1 $r1 1      # aloca espaço na pilha
413 - sw $r31 0($r1)       # empilha registrador
414 - subi $r1 $r1 1      # aloca espaço na pilha
415 - sw $r2 0($r1)       # empilha registrador
416 - move $r2 $r1         # fp = sp
417 - li $r46 32 # argument 0 (32)
418 - msgLcd $r46
419 - nop 0
420 - move $r1 $r2         # restaura stack pointer
421 - lw $r2 0($r1)       # desempilha registrador
422 - addi $r1 $r1 1       #desaloca espaço na pilha
423 - lw $r31 0($r1)       # desempilha registrador
424 - addi $r1 $r1 1       #desaloca espaço na pilha
425 - move $r0 $r0
426 - jr $r31         # retorna (void function end)
427 - receiveUART: # nova função receiveUART
428 - subi $r1 $r1 1      # aloca espaço na pilha
429 - sw $r31 0($r1)       # empilha registrador
430 - subi $r1 $r1 1      # aloca espaço na pilha
431 - sw $r2 0($r1)       # empilha registrador
432 - move $r2 $r1         # fp = sp
433 - subi $r1 $r1 1 # aloca espaço para variável 'pronto'
434 - sw $r63 0($r1)  # inicializa com 0
435 - move $r5 $r1   # endereço da variável 'pronto'
436 - subi $r1 $r1 1 # aloca espaço para variável 'aux'
437 - sw $r63 0($r1)  # inicializa com 0
438 - move $r6 $r1   # endereço da variável 'aux'
439 - subi $r1 $r1 1 # aloca espaço para variável 'header'
440 - sw $r63 0($r1)  # inicializa com 0
441 - move $r7 $r1   # endereço da variável 'header'
442 - subi $r1 $r1 1 # aloca espaço para variável 'flag'
443 - sw $r63 0($r1)  # inicializa com 0
444 - move $r8 $r1   # endereço da variável 'flag'
445 - subi $r1 $r1 1 # aloca espaço para variável 'comando'
446 - sw $r63 0($r1)  # inicializa com 0
447 - move $r9 $r1   # endereço da variável 'comando'
448 - subi $r1 $r1 1 # aloca espaço para variável 'valor'
449 - sw $r63 0($r1)  # inicializa com 0
450 - move $r10 $r1   # endereço da variável 'valor'
451 - subi $r1 $r1 1 # aloca espaço para variável 'checksum'
452 - sw $r63 0($r1)  # inicializa com 0
453 - move $r11 $r1   # endereço da variável 'checksum'
454 - li $r46 33 # argument 0 (33)
455 - msgLcd $r46
456 - li $r16 1000
457 - sw $r16 0($r6) # movendo t1 para aux
458 - li $r15 0
459 - sw $r15 0($r5) # movendo t2 para pronto
460 - L17: #Nova Label L17
461 - lw $r13 0($r5) # movendo pronto para t3
462 - li $r16 0
463 - addil $r43 $r44 L18
464 - beq $r13 $r16 L18 # jump se t3 == t4
465 - addil $r43 $r44 L19
466 - j L19
467 - L18: #Nova Label L18
468 - lw $r46 0($r6)    # carrega valor da variável local 'aux' para argument 0
469 - lw $r17 0($r46)
470 - sw $r17 0($r5) # movendo t7 para pronto
471 - addil $r43 $r44 L17
472 - j L17
473 - L19: #Nova Label L19
474 - lw $r16 0($r6) # movendo aux para t8
475 - li $r17 1
476 - add $r14 $r16 $r17 # salva em t10 (r14) 
477 - move $r46 $r14 # argument 0 (t10)
478 - li $r47 1 # argument 1 (1)
479 - sw $r47 0($r46)
480 - lw $r19 0($r6) # movendo aux para t12
481 - li $r20 2
482 - add $r18 $r19 $r20 # salva em t14 (r18) 
483 - move $r46 $r18 # argument 0 (t14)
484 - lw $r19 0($r46)
485 - sw $r19 0($r7) # movendo t15 para header
486 - lw $r19 0($r6) # movendo aux para t16
487 - li $r21 3
488 - add $r20 $r19 $r21 # salva em t18 (r20) 
489 - move $r46 $r20 # argument 0 (t18)
490 - lw $r19 0($r46)
491 - sw $r19 0($r8) # movendo t19 para flag
492 - lw $r19 0($r6) # movendo aux para t20
493 - li $r21 4
494 - add $r4 $r19 $r21 # salva em t22 (r4) 
495 - move $r46 $r4 # argument 0 (t22)
496 - lw $r19 0($r46)
497 - sw $r19 0($r9) # movendo t23 para comando
498 - lw $r19 0($r9) # movendo comando para t24
499 - sw $r19 0($r35) # movendo t24 para comandoUart
500 - lw $r19 0($r6) # movendo aux para t25
501 - li $r22 5
502 - add $r21 $r19 $r22 # salva em t27 (r21) 
503 - move $r46 $r21 # argument 0 (t27)
504 - lw $r19 0($r46)
505 - sw $r19 0($r10) # movendo t28 para valor
506 - lw $r19 0($r6) # movendo aux para t29
507 - li $r23 6
508 - add $r22 $r19 $r23 # salva em t31 (r22) 
509 - move $r46 $r22 # argument 0 (t31)
510 - lw $r19 0($r46)
511 - sw $r19 0($r11) # movendo t32 para checksum
512 - li $r46 500 # argument 0 (500)
513 - lw $r47 0($r7)    # carrega valor da variável local 'header' para argument 1
514 - sw $r47 0($r46)
515 - li $r46 501 # argument 0 (501)
516 - lw $r47 0($r8)    # carrega valor da variável local 'flag' para argument 1
517 - sw $r47 0($r46)
518 - li $r46 502 # argument 0 (502)
519 - lw $r47 0($r9)    # carrega valor da variável local 'comando' para argument 1
520 - sw $r47 0($r46)
521 - li $r46 503 # argument 0 (503)
522 - lw $r47 0($r10)    # carrega valor da variável local 'valor' para argument 1
523 - sw $r47 0($r46)
524 - li $r46 504 # argument 0 (504)
525 - lw $r47 0($r11)    # carrega valor da variável local 'checksum' para argument 1
526 - sw $r47 0($r46)
527 - nop 0
528 - move $r1 $r2         # restaura stack pointer
529 - lw $r2 0($r1)       # desempilha registrador
530 - addi $r1 $r1 1       #desaloca espaço na pilha
531 - lw $r31 0($r1)       # desempilha registrador
532 - addi $r1 $r1 1       #desaloca espaço na pilha
533 - move $r0 $r0
534 - jr $r31         # retorna (void function end)
535 - checkFlags: # nova função checkFlags
536 - subi $r1 $r1 1      # aloca espaço na pilha
537 - sw $r31 0($r1)       # empilha registrador
538 - subi $r1 $r1 1      # aloca espaço na pilha
539 - sw $r2 0($r1)       # empilha registrador
540 - move $r2 $r1         # fp = sp
541 - lw $r52 2($r2)      # carrega param 0
542 - subi $r1 $r1 1 # aloca espaço para variável 'valorAns'
543 - sw $r63 0($r1)  # inicializa com 0
544 - move $r5 $r1   # endereço da variável 'valorAns'
545 - move $r6 $r52 # movendo flag para t0
546 - li $r8 1
547 - addil $r43 $r44 L20
548 - bnq $r6 $r8 L20 # jump se t0 != t1
549 - li $r46 2000 # argument 0 (2000)
550 - lw $r12 0($r46)
551 - sw $r12 0($r5) # movendo t4 para valorAns
552 - addil $r43 $r44 L21
553 - j L21
554 - L20: #Nova Label L20
555 - move $r8 $r52 # movendo flag para t5
556 - li $r13 2
557 - addil $r43 $r44 L22
558 - bnq $r8 $r13 L22 # jump se t5 != t6
559 - li $r46 2001 # argument 0 (2001)
560 - lw $r15 0($r46)
561 - sw $r15 0($r5) # movendo t9 para valorAns
562 - addil $r43 $r44 L23
563 - j L23
564 - L22: #Nova Label L22
565 - move $r13 $r52 # movendo flag para t10
566 - li $r16 3
567 - addil $r43 $r44 L24
568 - bnq $r13 $r16 L24 # jump se t10 != t11
569 - li $r46 2002 # argument 0 (2002)
570 - lw $r18 0($r46)
571 - sw $r18 0($r5) # movendo t14 para valorAns
572 - addil $r43 $r44 L25
573 - j L25
574 - L24: #Nova Label L24
575 - li $r16 0
576 - sw $r16 0($r5) # movendo t15 para valorAns
577 - nop 0
578 - L25: #Nova Label L25
579 - L23: #Nova Label L23
580 - L21: #Nova Label L21
581 - lw $r16 0($r5) # movendo valorAns para t16
582 - move $r45 $r16 # move valor de retorno para v0
583 - move $r1 $r2         # restaura stack pointer
584 - lw $r2 0($r1)       # desempilha registrador
585 - addi $r1 $r1 1       #desaloca espaço na pilha
586 - lw $r31 0($r1)       # desempilha registrador
587 - addi $r1 $r1 1       #desaloca espaço na pilha
588 - jr $r31         # retorna
589 - sendUART: # nova função sendUART
590 - subi $r1 $r1 1      # aloca espaço na pilha
591 - sw $r31 0($r1)       # empilha registrador
592 - subi $r1 $r1 1      # aloca espaço na pilha
593 - sw $r2 0($r1)       # empilha registrador
594 - move $r2 $r1         # fp = sp
595 - subi $r1 $r1 1 # aloca espaço para variável 'comando'
596 - sw $r63 0($r1)  # inicializa com 0
597 - move $r5 $r1   # endereço da variável 'comando'
598 - subi $r1 $r1 1 # aloca espaço para variável 'flag'
599 - sw $r63 0($r1)  # inicializa com 0
600 - move $r6 $r1   # endereço da variável 'flag'
601 - subi $r1 $r1 1 # aloca espaço para variável 'valorRec'
602 - sw $r63 0($r1)  # inicializa com 0
603 - move $r7 $r1   # endereço da variável 'valorRec'
604 - subi $r1 $r1 1 # aloca espaço para variável 'valorAns'
605 - sw $r63 0($r1)  # inicializa com 0
606 - move $r8 $r1   # endereço da variável 'valorAns'
607 - subi $r1 $r1 1 # aloca espaço para variável 'header'
608 - sw $r63 0($r1)  # inicializa com 0
609 - move $r9 $r1   # endereço da variável 'header'
610 - subi $r1 $r1 1 # aloca espaço para variável 'checksum'
611 - sw $r63 0($r1)  # inicializa com 0
612 - move $r10 $r1   # endereço da variável 'checksum'
613 - li $r46 34 # argument 0 (34)
614 - msgLcd $r46
615 - li $r46 501 # argument 0 (501)
616 - lw $r15 0($r46)
617 - sw $r15 0($r6) # movendo t2 para flag
618 - li $r46 502 # argument 0 (502)
619 - lw $r16 0($r46)
620 - sw $r16 0($r5) # movendo t4 para comando
621 - li $r46 503 # argument 0 (503)
622 - lw $r17 0($r46)
623 - sw $r17 0($r7) # movendo t6 para valorRec
624 - lw $r17 0($r5) # movendo comando para t7
625 - li $r18 1
626 - addil $r43 $r44 L26
627 - bnq $r17 $r18 L26 # jump se t7 != t8
628 - li $r20 124
629 - sw $r20 0($r9) # movendo t10 para header
630 - lw $r19 0($r7) # movendo valorRec para t11
631 - sw $r19 0($r8) # movendo t11 para valorAns
632 - addil $r43 $r44 L27
633 - j L27
634 - L26: #Nova Label L26
635 - lw $r18 0($r5) # movendo comando para t12
636 - li $r20 2
637 - addil $r43 $r44 L28
638 - bnq $r18 $r20 L28 # jump se t12 != t13
639 - li $r22 124
640 - sw $r22 0($r9) # movendo t15 para header
641 - lw $r21 0($r7) # movendo valorRec para t16
642 - sw $r21 0($r8) # movendo t16 para valorAns
643 - addil $r43 $r44 L29
644 - j L29
645 - L28: #Nova Label L28
646 - lw $r20 0($r5) # movendo comando para t17
647 - li $r22 3
648 - addil $r43 $r44 L30
649 - bnq $r20 $r22 L30 # jump se t17 != t18
650 - li $r24 124
651 - lw $r23 0($r6) # movendo flag para t21
652 - add $r25 $r24 $r23 # salva em t22 (r25) 
653 - sw $r25 0($r9) # movendo t22 para header
654 - subi $r1 $r1 1  # aloca espaço para 1 argumentos
655 - lw $r46 0($r6)    # carrega valor da variável local 'flag' para argument 0
656 - sw $r46 0($r1)  # salva argument 0 na pilha
657 - addil $r43 $r44 checkFlags
658 - jal checkFlags
659 - addi $r1 $r1 1  # libera espaço de 1 argumentos
660 - lw $r5 -1($r2)  # recarrega variavel local
661 - lw $r6 -2($r2)  # recarrega variavel local
662 - lw $r7 -3($r2)  # recarrega variavel local
663 - lw $r8 -4($r2)  # recarrega variavel local
664 - lw $r9 -5($r2)  # recarrega variavel local
665 - lw $r10 -6($r2)  # recarrega variavel local
666 - move $r24 $r45 # copia retorno (v0) para t24
667 - sw $r24 0($r8) # movendo t24 para valorAns
668 - addil $r43 $r44 L31
669 - j L31
670 - L30: #Nova Label L30
671 - nop 0
672 - L31: #Nova Label L31
673 - L29: #Nova Label L29
674 - L27: #Nova Label L27
675 - li $r46 1008 # argument 0 (1008)
676 - lw $r47 0($r9)    # carrega valor da variável local 'header' para argument 1
677 - sw $r47 0($r46)
678 - li $r46 1009 # argument 0 (1009)
679 - lw $r47 0($r5)    # carrega valor da variável local 'comando' para argument 1
680 - sw $r47 0($r46)
681 - li $r46 1010 # argument 0 (1010)
682 - lw $r47 0($r8)    # carrega valor da variável local 'valorAns' para argument 1
683 - sw $r47 0($r46)
684 - li $r46 1011 # argument 0 (1011)
685 - li $r47 1 # argument 1 (1)
686 - sw $r47 0($r46)
687 - nop 0
688 - move $r1 $r2         # restaura stack pointer
689 - lw $r2 0($r1)       # desempilha registrador
690 - addi $r1 $r1 1       #desaloca espaço na pilha
691 - lw $r31 0($r1)       # desempilha registrador
692 - addi $r1 $r1 1       #desaloca espaço na pilha
693 - move $r0 $r0
694 - jr $r31         # retorna (void function end)
695 - main: # nova função main
696 - addil $r43 $r44 mapeamento
697 - jal mapeamento
698 - li $r46 0 # argument 0 (0)
699 - msgLcd $r46
700 - nop 0
701 - li $r6 0
702 - move $r44 $r6 # movendo t1 para registrador especial salto
703 - li $r7 3
704 - move $r60 $r7 # movendo t2 para registrador especial processosCarregados
705 - nop 0
706 - nop 0
707 - addil $r43 $r44 menuShell
708 - jal menuShell
709 - nop 0
710 - li $r8 0
711 - move $r44 $r8 # movendo t3 para registrador especial salto
712 - nop 0
713 - L32: #Nova Label L32
714 - move $r9 $r41 # movendo opcao para registrador especial t4
715 - li $r11 6
716 - addil $r43 $r44 L33
717 - bnq $r9 $r11 L33 # jump se !=
718 - addil $r43 $r44 L34
719 - j L34
720 - L33: #Nova Label L33
721 - addil $r43 $r44 defineProcesso
722 - jal defineProcesso
723 - addil $r43 $r44 initDados
724 - jal initDados
725 - addil $r43 $r44 initUART
726 - jal initUART
727 - addil $r43 $r44 receiveUART
728 - jal receiveUART
729 - lw $r11 0($r35) # movendo comandoUart para t7
730 - li $r13 3
731 - addil $r43 $r44 L35
732 - bnq $r11 $r13 L35 # jump se t7 != t8
733 - addil $r43 $r44 sendUART
734 - jal sendUART
735 - addil $r43 $r44 L36
736 - j L36
737 - L35: #Nova Label L35
738 - nop 0
739 - addil $r43 $r44 execucaoNP
740 - jal execucaoNP
741 - addil $r43 $r44 votenaoPreemptivo
742 - jal votenaoPreemptivo
743 - addil $r43 $r44 sendUART
744 - jal sendUART
745 - L36: #Nova Label L36
746 - nop 0
747 - li $r13 0
748 - move $r44 $r13 # movendo t10 para registrador especial salto
749 - addil $r43 $r44 menuShell
750 - jal menuShell
751 - addil $r43 $r44 L32
752 - j L32
753 - L34: #Nova Label L34
754 - nop 0
755 - nop 0
756 - nop 0
757 - li $r46 28 # argument 0 (28)
758 - msgLcd $r46
759 - nop 0
760 - nop 0
761 - nop 0
762 - nop 0
763 - halt # termina a execução
