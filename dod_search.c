
// Binary search to find group.
static i64 find_index(i64 * indexes, i64 size, i64 index){
  i64 distance = 100;
  i64 start = 0;
  i64 search_bounds = (size + 1) / 2;
  if(size > index){
    i64 first_index = indexes[index];
    distance = index - first_index;
    if(distance == 0)
      return index;
    if(distance < search_bounds){
      search_bounds = distance;
      start = first_index + distance;
    }
  }

  while(true){

    i64 next = search_bounds + start;
    logd("%i %i %i %i\n", search_bounds, start, next, indexes[next]);
    if(indexes[next] == index)
      return next;
    if(indexes[next] > index){
      search_bounds = - (search_bounds + 1) / 2;
    }
    if(indexes[next] < index)
      search_bounds = search_bounds / 2;
    if(search_bounds == 0)
      return -1;
    start = next;
  }
  UNREACHABLE();
  return -1;
}

/*static i64 find_index_interp(i64 * indexes, i64 size, i64 index){
  
  i64 a = index;
  i64 s1;
  i64 b;
  i64 d;
  i64 s2;
  i64 c;
  while(true){
    s1 = MIN(a, size - 1);
    b = indexes[s1];
    if(b == index) return s1;
    d = b - s1;
    s2 = MIN(MAX(0, b), size -1);
    c = indexes[s2];
    if(c == index) return s2;
    double delta = (s1 - s2) / (double) (b - c);
    a = (index - c) * delta + s1;
    logd("%i %i %i %i %i %f\n", s1, s2, b, c, d, delta);
    if(a == s1) return -1;
  }
  UNREACHABLE();
  return -1;
  }*/

static i64 find_index_interp2(i64 * indexes, i64 size, i64 index){
  i64 i1 = 0, i2 = size -1, a1 = indexes[i1], a2 = indexes[i2];
  i64 a = ((index - a1) * size) / (a2 - a1);
  i64 binerror = 2 * (size / 4);
  i64 pa = a;
  while(true){
    i64 new_index = a;
    if(new_index < 0)
      new_index = 0;
    else if (new_index >= size)
      new_index = size - 1;
    i64 new_value = indexes[new_index];
    i64 error = new_value - index;
    if(error == 0)
      return new_index;
    else if(error < 0){
      if(binerror < -error)
	error = -binerror;
      else
	binerror = -error;
    }else{
      if(binerror < error)
	error = binerror;
      else
	binerror = error;
    }
    pa = a;
    a = new_index - error;
    if(a == pa) return -1;
    if(binerror > 1)
      binerror /= 2;
  }
  UNREACHABLE();
  return -1;
}

static i64 find_indexe_interp4(i64 * indexes, i64 size, i64 * index, i64 index_cnt, i64 * out_buffer){
  for(i64 i = 0; i < index_cnt; i++){
    i64 thisindex = index[i];
  }
}

static i64 find_index_interp3(i64 * indexes, i64 size, i64 index){
  i64 a = size / 2;
  i64 binerror = size / 2;
  while(true){
    i64 new_index = a;
    if(new_index < 0) new_index = 0;
    else if(new_index >= size) new_index = size -1;
    i64 new_value = indexes[new_index];
    i64 error = new_value - index;
    if(error < 0)
      a = new_index + binerror;
    else if (error > 0)
      a = new_index - binerror;
    else
      return new_index;
    binerror = binerror >= 2 ? (binerror / 2) : 1;
  }
  UNREACHABLE();
  return -1;
}
