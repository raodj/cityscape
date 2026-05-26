#!/usr/bin/awk -f

# Field positions: $1=per, $2=perID, $3=HldID, $4=#info(=4), $5=age, $6=wage, $7=JWMNP, $8=JWTRNS, $9…NF = the "…schedule…"


BEGIN { 
  print "Format per line: MODE|BUCKET  TOTAL  MISSING" 
  print "BUCKET VALUE: 0 = 0–10 min, 1 = 10–20, 2 = 20–30, 3 = 30–40, 4 = 40–50, 5 = 50–60, 6 = 60–70, 7 = 70–80, 8 = 80–90, 9 = 90-100, 10 = 100-110, 11 = 110–120"
}

/^per / {
  jwmnp=$7+0; jwtrns=$8+0
  if (jwtrns!=1 && jwtrns!=2 && jwtrns!=7 && jwtrns!=8) next
  if (jwmnp < 0) next
  # Schedule = everything after $8, joined with spaces; empty schedule is the literal ""
  sched=""
  for (i=9;i<=NF;i++) sched=sched (i>9?" ":"") $i
  has = (sched != "\"\"")
  b = int(jwmnp/10); if (b>15) b=15      # cap at bin 15 (150+ min)
  key=jwtrns"|"b
  bk_tot[key]++
  if (!has) bk_miss[key]++
}
END {
  for (k in bk_tot) print k, bk_tot[k], bk_miss[k]+0
}


/^per / {
  jwmnp=$7+0; jwtrns=$8+0
  if (jwtrns!=1 && jwtrns!=2 && jwtrns!=7 && jwtrns!=8) next
  if (jwmnp < 0) next
  sched=""
  for (i=9;i<=NF;i++) sched=sched (i>9?" ":"") $i
  has = (sched != "\"\"")
  tot_n[jwtrns]++; tot_s[jwtrns]+=jwmnp
  if (jwmnp < min[jwtrns] || tot_n[jwtrns]==1) min[jwtrns]=jwmnp
  if (jwmnp > max[jwtrns] || tot_n[jwtrns]==1) max[jwtrns]=jwmnp
  if (has) { a_n[jwtrns]++; a_s[jwtrns]+=jwmnp }
  else     { m_n[jwtrns]++; m_s[jwtrns]+=jwmnp }
}
END {
  printf("%-4s %8s %8s %8s %12s %12s %s\n",
          "JWT","total","assigned","missing","mean_all","mean_miss","range")
  for (k in tot_n) {
    mu_all = tot_s[k]/tot_n[k]
    mu_mis = (m_n[k] ? m_s[k]/m_n[k] : 0)
    printf("%-4d %8d %8d %8d %12.1f %12.1f [%d, %d]\n",
            k, tot_n[k], a_n[k]+0, m_n[k]+0, mu_all, mu_mis, min[k], max[k])
  }
}

