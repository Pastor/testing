int spc_throttle(int *attempts, int max_attempts, int allowed_fails, int delay) {
  int exp;

  (*attempts)++;
  if (*attempts > max_attempts) return -1;
  if (*attempts <= allowed_fails) return 0;
  for (exp = *attempts - allowed_fails - 1;  exp;  exp--)
    delay *= 2;
  return delay;
}
