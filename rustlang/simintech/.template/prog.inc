/*  ------------------------------------------------------
     Routine name:  my_diagram
     Description:   
     Project file:  2.prt

------------------------------------------------------  */

/*       Local stack variables                */
int i;
int j;
int c;
int itmp1;
int itmp2;
double tmp1;
double tmp2;
double tmp3;
double tmp4;
double tmp5;
double tmp6;
double tmp7;
char f;
char tmp_f_1;
char u_s;
char u_r;
ret = 0;

switch (action){
case f_Stop:{
/* Index=4
   UID=4
   GeneratorClassName=TLanguage
   Name=LangBlock22
   Type=язык программировани€ */

v4_sfinal__0:
;
};break;
case f_GetDeri:{
};break;
case f_GetAlgFun:{
};break;
default:{

/* Index=0
   UID=0
   GeneratorClassName=TInputPin
   Name=Const_source18
   Type=¬ходной контакт */

state_vars->my_diagramv0_out_0_ = ((input_0));

/* Index=1
   UID=1
   GeneratorClassName=TInputPin
   Name=Const_source21
   Type=¬ходной контакт */

state_vars->my_diagramv1_out_0_ = ((input_0));

/* Index=2
   UID=2
   GeneratorClassName=TInputPin
   Name=Const_source19
   Type=¬ходной контакт */

state_vars->my_diagramv2_out_0_ = ((input_1));

/* Index=3
   UID=3
   GeneratorClassName=TInputPin
   Name=Const_source20
   Type=¬ходной контакт */

state_vars->my_diagramv3_out_0_ = ((input_2));
/* Index=4
   UID=4
   GeneratorClassName=TLanguage
   Name=LangBlock22
   Type=язык программировани€ */

state_vars->my_diagramv4_out_0_=((state_vars->my_diagramv1_out_0_+state_vars->my_diagramv2_out_0_)+state_vars->my_diagramv3_out_0_);
state_vars->my_diagramv4_out_1_=(state_vars->my_diagramv4_out_0_+(1));
v4___2:
;

/* Index=5
   UID=5
   GeneratorClassName=TOutPin
   Name=OutPin15
   Type=¬ыходной контакт */

if(isfinite(state_vars->my_diagramv4_out_0_)){
out_0 = state_vars->my_diagramv4_out_0_;
};

/* Index=6
   UID=6
   GeneratorClassName=TOutPin
   Name=OutPin16
   Type=¬ыходной контакт */

if(isfinite(state_vars->my_diagramv4_out_1_)){
out_2 = state_vars->my_diagramv4_out_1_;
};
};break;
};
