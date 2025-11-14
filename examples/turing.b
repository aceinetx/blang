W;

print_tape(tape, N){
  extrn printf, putchar;
  auto i;
  i = 0;
  while(i++ < N){
    printf("%ld ", tape[i-1]);
  }
  putchar(10);
}

increment(){
  extrn malloc, free;
  auto tape, count, i;
  tape = malloc(50 * W);

  tape[0] = 0;
  tape[1] = 0;
  tape[2] = 0;
  tape[3] = 0;

  count = 16;
  while(count--){
    i = 3;
    while(i >= 0){
      if(tape[i] == 0){
        tape[i] = 1;
        break;
      } else if(tape[i] == 1){
        tape[i] = 0;
      }

      i--;
    }

    print_tape(tape, 4);
  }

  free(tape);
}

copy(){
  extrn malloc, free;
  auto tape;
  tape = malloc(50 * W);

  tape[0] = 1;
  tape[1] = 0;
  tape[2] = 1;

  print_tape(tape, 3);

  memcpy(&tape[3], tape, 3 * W);

  print_tape(tape, 6);

  free(tape);
}

is_even(){
  extrn malloc, free;
  auto tape;
  tape = malloc(50 * W);

  tape[0] = 1;
  tape[1] = 0;
  tape[2] = 1;

  print_tape(tape, 3);

  if(tape[2]){
    puts("number is ODD");
  } else {
    puts("number is EVEN");
  }

  free(tape);
}

clear(){
  extrn malloc, free;
  auto tape;
  tape = malloc(50 * W);

  tape[0] = 1;
  tape[1] = 0;
  tape[2] = 1;

  print_tape(tape, 3);
  memset(tape, 0, W*3);
  print_tape(tape, 3);

  free(tape);
}

contains(){
  extrn malloc, free;
  auto tape, i;
  tape = malloc(50 * W);

  tape[0] = 1;
  tape[1] = 0;
  tape[2] = 1;
  tape[3] = 1;
  tape[4] = 0;
  tape[5] = 1;
  tape[6] = 1;
  tape[7] = 1;

  i = 0;
  while(i < 7){
    if(tape[i] & tape[i+1]){
      printf("found at %d*n", i);
    }

    i++;
  }

  free(tape);
}

main(){
  extrn puts;
  W = &0[1];
  puts("increment");
  increment();
  puts("copy");
  copy();
  puts("is_even");
  is_even();
  puts("clear");
  clear();
  puts("contains");
  contains();
}