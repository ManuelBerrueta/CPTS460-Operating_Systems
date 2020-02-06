int mystrlen(char *str)
{
    int i = 0;

    while(str[i] != 0)
    {
        i++;
    }

    return i;
}


int mystrcmp(char *left, char *right)
{ 
    int i = 0;

    if (mystrlen(left) != mystrlen(right))
    {
        return 1;
    }

    
    while (left[i] != 0)
    {
        if (left[i] != right[i])
        {
            return 1;
        }
        i++;
    }

    return 0;
}


int mystrcpy(char *in, char *out)
{
  while(*in)
  {
    *out++ = *in++;
  }
  *out = 0;
}
