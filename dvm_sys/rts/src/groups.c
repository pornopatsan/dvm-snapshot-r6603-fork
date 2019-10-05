#ifndef  _GROUPS_C_
#define  _GROUPS_C_
/*****************/    /*E0000*/

/**************************\
*  Names of events groups  *
\**************************/    /*E0001*/
 
void  groups(void)
{
  stat_grp(UserGrp);
  stat_grp(MsgPasGrp);
  stat_grp(StartRedGrp);
  stat_grp(WaitRedGrp);
  stat_grp(RedGrp);
  stat_grp(StartShdGrp);
  stat_grp(WaitShdGrp);
  stat_grp(ShdGrp);
  stat_grp(DistrGrp);
  stat_grp(ReDistrGrp);
  stat_grp(MapPLGrp);
  stat_grp(DoPLGrp);
  stat_grp(ProgBlockGrp);
  stat_grp(IOGrp);
  stat_grp(RemAccessGrp);
  stat_grp(UserDebGrp);
  stat_grp(StatistGrp);
  stat_grp(SystemGrp);

  return;
}

#endif   /*  _GROUPS_C_  */    /*E0002*/
