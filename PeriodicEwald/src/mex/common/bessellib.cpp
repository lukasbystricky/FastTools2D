/*
 * Functions to compute modified bessel of second kind, K0(x) and K1(x),
 * as well as the incomplete modified bessel functions of second kind, K0(x,y) and K1(x,y).
 *
 * Algorithms from Harris and Fripiat (2008)
 */
#include "bessellib.h"



/* -----------------------------------------------------------------------/
 * Computes K1(x,y)
 *-----------------------------------------------------------------------*/
double K1xy(double x, double y, int alg, const int jmax) {
    
    // Define u and v for future usage
    double u = sqrt(x*y);
    double v = sqrt(x/y);
    
    double temp = 0;
    
    // If y==0. K1(x,0) = E2(x)
    if (fabs(y) < 1e-15) {
        return E2(x);
    }
    
    if (x > 500) {
        return 0;
    }
    
    
    // Alg. 3 if y and y/x are large --> compute K1(x,y) = 2vK1(2u) - K-1(y,x)
    if (alg == 3) {
        int jmax = choose_algorithm(y,x,&alg);
        temp = Km1xy(y,x,alg,jmax);
        return 2.*v*K1x(2.*u)-temp;
    }
    else if (alg == 2) {
        temp = K1x(2.*u);
        
        double v1j = v-1.;
        double s=0.;
        
        for (int j=1; j<=jmax; j++) {
            s+=(Cjnu(j,1,u)*v1j);
            v1j*=(v-1.);
        }
        return v*(temp+exp(-2.*u)*(-0.5/u + s));
    }
    
    else if (alg == 1) {
     
        double UJ[jmax+1];
        UJ[0] = 1.;
        UJ[1] = exp(x)*E2(x);
        // j=0
        double s = 0;
        double yj = 1;
        // j=1
        s += yj*UJ[1];
        yj *= y;
        // j=2...jmax+1
        for (int j=1; j<= jmax; j++) {
            UJ[j+1] = (double) ((-UJ[j-1]-(0.-2*j-x)*UJ[j])/(j*(j+1.-0.)));
            s += yj*UJ[j+1];
            yj *= y;
        }
        return (exp(-(x+y))*s);
        
    }
    return 0;
}

/* -----------------------------------------------------------------------/
 * Computes K_-1(x,y)
 *-----------------------------------------------------------------------*/
double Km1xy(double x, double y, int alg, const int jmax) {
    
    //If y==0. Km1(x,0) = E0(x)
    if (fabs(y) < 1e-15) {
        return E0(x);
    }
    
    if (x>500) {
        return 0;
    }
    
    double UJ[jmax+1];
    UJ[0] = 1.;
    // j=0
    double s = 0.;
    double yj = 1.;
    UJ[1] = 1./x;
    // j=1
    s += yj*UJ[1];
    yj *= y;
    UJ[2] = 1./x - exp(x)*E1(x);
    // j=2
    s += yj*UJ[2];
    yj *= y;
    for (int j=2; j<=jmax; j++) {
        UJ[j+1] = (double) ((-UJ[j-1]-(2.-2*j-x)*UJ[j])/(j*(j+1.-2.)));
        s += yj*UJ[j+1];
        yj *= y;
    }
    return (exp(-(x+y))*s);
    
}


/* -----------------------------------------------------------------------/
 * Computes K0(x,y)
 *-----------------------------------------------------------------------*/
double K0xy(double x, double y, int alg, const int jmax) {
    
    // Define u and v for future usage
    double u = sqrt(x*y);
    double v = sqrt(x/y);
    
    double temp = 0;                                                         // Temporary return variable
    
    // If y==0. K0(x,0) = E1(x)
    if (fabs(y) < 1e-15) {
        //double tmp = gsl_sf_expint_E1(x);
        double tmp = E1(x);
        return tmp;
    }
    
    if (x > 500) {  // This seems ok, K0 decreases rapidly with x.
        return 0;
    }
    
    // Alg. 3 if y and y/x are large --> compute K0(x,y) = 2K0(2*sqrt(x*y)) - K0(y,x)
    if (alg == 3) {
        int jmax = choose_algorithm(y,x,&alg);
        temp = K0xy(y,x,alg,jmax);
        return 2.*K0x(2.*u)-temp;
    }
    else if (alg == 2) {
        temp = K0x(2.*u);
        
        double v1j = v-1.;
        double s=0.;
        
        for (int j=1; j<=jmax; j++) {
            s+=(Cjnu(j,0,u)*v1j);
            v1j*=(v-1.);
        }
        return temp+exp(-2.*u)*s;
    }
    
    else if (alg == 1) {
        double UJ[jmax+1];
        UJ[0] = 1.;
        UJ[1] = exp(x)*E1(x);
//         UJ[1] = exp(x)*gsl_sf_expint_E1(x);
        double yj = 1.;
        double s = yj*UJ[1];
        yj *= y;
        for (int j=1; j<= jmax; j++) {
            UJ[j+1] = (double) (-UJ[j-1]-(1.-2.*j-x)*UJ[j])/( (double) j*j);
            s += yj*UJ[j+1];
            yj *= y;
        }
        return (exp(-(x+y))*s);
    }
    return 0;
}


/* -----------------------------------------------------------------------/
 * Chooses which algorithm to use to compute Kn(x,y)
 *-----------------------------------------------------------------------*/
int choose_algorithm(double x, double y, int *algorithm) {
    int jmax=0;
    int alg=0;
    
    if (fabs(y-x)<1e-12) {
        alg = 2;
        jmax = ceil(15.-x);
    }
    else if (y<1.5) {
        alg = 1;
        jmax = ceil(15.-x);
    }
    else if (y>=1.5 && y<2.) {
        if (x<(2.*y-3.)) {
            alg = 3;
            jmax = ceil(8.-6.*x);
        }
        else {
            alg = 1;
            jmax = ceil(15.-x);
        }
    }
    else if ( y>=2. && y<4.) {
        if (x>y) {
            alg = 1;
            jmax = ceil(15.-x);
        }
        else {
            alg = 3;
            jmax = ceil(10.-y+12.*x/y);
        }
    }
    else if (y>=4. && y<6.) {
        if(x>y) {
            alg=1;
            jmax = ceil(16.-x);
        }
        else {
            alg = 3;
            jmax = ceil(10.-y+12.*x/y);
        }
    }
    else if (y>=6. && y<9.) {
        if (x>y) {
            alg = 1;
            jmax = ceil(16.-x);
        }
        else {
            alg = 3;
            jmax = ceil(10.-y+12.*x/y);
        }
    }
    else if (y>=9. && y<10.) {
        if (x>27-2*y) {
            alg = 1;
            jmax = ceil(16.-x);
        }
        else {
            alg = 3;
            jmax = ceil(3.+5./6.*x);
        }
    }
    else if (y>=10.) {
        if (x < y) {
            alg = 3;
            jmax = MIN( ceil(3.+x/1.2), ceil(19.-y) );
        }
        else if (x >= y && x < 3./2.*y) {
            alg = 2;
            jmax = ceil(15.-x);
        }
        else {
            alg = 1;
            jmax = MIN( ceil(3.+x/1.2), ceil(19.-y) );
        }
    }
    *algorithm=alg;
    if(jmax<1) jmax = 2;
    return jmax;
}


/* -----------------------------------------------------------------------/
 * Computes polynomials C_jn(u)
 *-----------------------------------------------------------------------*/
double Cjnu(double j, int n, double u) {
    int rndj = round((j-1.)/2.);
    double s=0.;
    double uk = 1.;
    
    for (int k=0; k<=rndj; k++) {
        s += (cjnk(j,n,k)*uk);
        uk *= u;
    }
    
    return s;
}

/* -----------------------------------------------------------------------/
 * Computes coefficients c_jn^k
 *-----------------------------------------------------------------------*/
double cjnk(int j, int n, int k) {
    int poch = Pochhammer(n+k+1,j-2*k-1);
    int kfac = factorial(k);
    int j2k1fac = factorial(j-2*k-1);
    double temp = 0.;
    if((j-k)%2==0) {
        temp =  (double) poch/(j*kfac*j2k1fac);
    }
    else {
        temp = (double) -poch/(j*kfac*j2k1fac);
    }
    return temp;
}

/* -----------------------------------------------------------------------/
 * Computes (a)_p = a*(a+1)*(a+2)*...*(a+p-1), with (a)_0 = 1 (Pochhammer)
 *-----------------------------------------------------------------------*/
double Pochhammer(int a, int p) {
    int s=1;
    double temp = 0.;
    if (p==0) {
        temp = 1.;
    }
    else {
        for (int j=0; j<p; j++) {
            s*=(a+j);
        }
        temp = s;
    }
    return temp;
}

/* -----------------------------------------------------------------------/
 * Computes the factorial of k, k! = 1*2*3*4*...*(k-1)*k
 *-----------------------------------------------------------------------*/
double factorial(int k) {
    int f = 1;
    if (k==0 || k==1) {
        return 1;
    }
    else {
        for (int j=1; j<=k; j++) {
            f*=j;
        }
        return f;
    }
}

/* -----------------------------------------------------------------------/
 * Computes the confluent hypergeometric function U(a,1-n,x) for n=0.
 *-----------------------------------------------------------------------*/
double Uj1x(int j, double x) {
    if (j==0) {
        return 1;                                                           // U(0,1-n,x) = 1
    }
    else if (j==1) {
        double tmp = E1(x);
//         double tmp = gsl_sf_expint_E1(x);
        return exp(x)*tmp;                                                // U(1,1-n,x) = exp(x)E_{n+1}(x)
    }
    else {
        double t1 = Uj1x(j-1,x);
        double t2 = Uj1x(j-2,x);
        
        return ((double) (-t2-(1.-2.*(j-1)-x)*t1)/((j-1)*(j-1)));
    }
}


/* -----------------------------------------------------------------------/
 * Computes the confluent hypergeometric function U(a,1-n,x) for n=-1.
 *-----------------------------------------------------------------------*/
double Uj2x(int j, double x) {
    if (j==0) {
        return 1;                                                           // U(0,1-n,x) = 1
    }
    else if (j==1) {
        return 1/x;                                                          // U(1,1-n,x) = exp(x)E_{n+1}(x)
    }
    else if (j==2) {
        double tmp = E1(x);
//         double tmp = gsl_sf_expint_E1(x);
        return 1/x-exp(x)*tmp;
    }
    else {
        
        double t1 = Uj2x(j-1,x);
        double t2 = Uj2x(j-2,x);
        
        double a = j-1;
        double b = 2;
        
        return (double) ((-t2-(b-2*a-x)*t1)/(a*(a+1-b)));
    }
}

/* -----------------------------------------------------------------------/
 * Computes the confluent hypergeometric function U(a,1-n,x) for n=1.
 *-----------------------------------------------------------------------*/
double Uj0x(int j, double x) {
    if (j==0) {
        return 1;
    }
    else if (j==1) {
        return exp(x)*E2(x);
    }
    else {
        double t1 = Uj0x(j-1,x);
        double t2 = Uj0x(j-2,x);
        
        double b = 0.;
        double a = j-1;
        
        return (double) ((-t2-(b-2*a-x)*t1)/(a*(a+1-b)));
    }
}

/* -----------------------------------------------------------------------/
 * Computes E2(x)
 *-----------------------------------------------------------------------*/
double E2(double x) {
    double tmp = E1(x);
//     double tmp = gsl_sf_expint_E1(x);
    return exp(-x)-x*tmp;
}

/* -----------------------------------------------------------------------/
 * Computes E0(x)
 *-----------------------------------------------------------------------*/
double E0(double x) {
    return exp(-x)/x;
}


/* -----------------------------------------------------------------------/
 * Computes E1(x)
 *-----------------------------------------------------------------------*/
double E1(double x) {
    int flag = 0;
    double sol=0.,c[10],t1;
    // Assign coefficients depending on regime
    if (x<3.8) {                                                // Regime 1
        flag = 1;
        if (x <= 0.5) {
            c[0] = 0.999999999999987808;
            c[1] = -0.249999999999065566;
            c[2] = 0.555555555300946082e-1;
            c[3] = -0.104166663232151262e-1;
            c[4] = 0.166666403220018885e-2;
            c[5] = -0.231469225196654398e-3;
            c[6] = 0.283093675470330591e-4;
            c[7] = -0.303840089096391965e-5;
            c[8] = 0.245205483628163801e-6;
        }
        else if (x > 0.5 && x <= 1.0) {
            c[0] = 0.999999999019877910;
            c[1] = -0.249999987549369342;
            c[2] = 0.555554856521454889e-1;
            c[3] = -0.104164388044599988e-1;
            c[4] = 0.166619075749750130e-2;
            c[5] = -0.230819366427626460e-3;
            c[6] = 0.277280365075701116e-4;
            c[7] = -0.272536286471831497e-5;
            c[8] = 0.166914146451689939e-6;
        }
        else if (x > 1.0 && x <= 1.5) {
            c[0] = 0.999999924025718761;
            c[1] = -0.249999435959948038;
            c[2] = 0.555536933436146687e-1;
            c[3] = -0.104130730905660952e-1;
            c[4] = 0.166218943796878826e-2;
            c[5] = -0.227731041992275497e-3;
            c[6] = 0.262149219321352965e-4;
            c[7] = -0.229471987308028015e-5;
            c[8] = 0.112380198080942536e-6;
        }
        else if (x > 1.5 && x <= 2.0) {
            c[0] = 0.999998859684899083;
            c[1] = -0.249993978063412804;
            c[2] = 0.555413747501954248e-1;
            c[3] = -0.103970827672667451e-1;
            c[4] = 0.164912783185956412e-2;
            c[5] = -0.220853767206681016e-3;
            c[6] = 0.239350012552983918e-4;
            c[7] = -0.185955681329646223e-5;
            c[8] = 0.757660026254469862e-7;
        }
        else if(x > 2.0 && x <= 2.6) {
            c[0] = 0.999990963871860813;
            c[1] = -0.249963498286574116;
            c[2] = 0.554896808406686866e-1;
            c[3] = -0.103467610165462097e-1;
            c[4] = 0.161836972871786630e-2;
            c[5] = -0.208764016560197358e-3;
            c[6] = 0.209504460749680938e-4;
            c[7] = -0.143643938874026765e-5;
            c[8] = 0.493910904409661228e-7;
        }
        else if(x > 2.6 && x <= 3.2) {
            c[0] = 0.999951600206057463;
            c[1] = -0.249844297780870831;
            c[2] = 0.553312357066131760e-1;
            c[3] = -0.102260006427169145e-1;
            c[4] = 0.156064427697809217e-2;
            c[5] = -0.191041181978835834e-3;
            c[6] = 0.175374318373428333e-4;
            c[7] = -0.105950124532254141e-5;
            c[8] = 0.311125668956286991e-7;
        }
        else if(x > 3.2 && x < 3.8) {
            c[0] = 0.999824244034471062;
            c[1] = -0.249529062464394609;
            c[2] = 0.549890774439250944e-1;
            c[3] = -0.100132857308537252e-1;
            c[4] = 0.147779681374040281e-2;
            c[5] = -0.170340713076035858e-3;
            c[6] = 0.142969778329726877e-4;
            c[7] = -0.768939332687689101e-6;
            c[8] = 0.196866577016285326e-7;
        }
    }
    else if (x >= 3.8 && x < 12.0) {                            // Regime 2
        flag = 2;
        if (x >= 3.8 && x <= 4.5) {
            c[0] = 0.999463791786992743;
            c[1] = -0.982095411910903118;
            c[2] = 1.726010340627460545;
            c[3] = -3.381582118371368059;
            c[4] = 5.464989381360068984;
            c[5] = -5.776428129402645378;
            c[6] = 2.857629406830988139;
        }
        else if(x > 4.5 && x <= 5.6) {
            c[0] = 0.999729513844049050;
            c[1] = -0.989265820291427478;
            c[2] = 1.806809757165159322;
            c[3] = -3.868249199940919894;
            c[4] = 7.117468222087097744;
            c[5] = -8.775513526474370119;
            c[6] = 5.130470691295011555;
        }
        else if(x > 5.6 && x <= 8) {
            c[0] = 0.999906553722766512;
            c[1] = -0.995221056899015174;
            c[2] = 1.890649133820455587;
            c[3] = -4.500560398706852311;
            c[4] = 9.811830054300493415;
            c[5] = -14.925387884677880200;
            c[6] = 11.004039217328305295;
        }
        else if(x > 8 && x < 12) {
            c[0] = 0.999982652460136059;
            c[1] = -0.998717547422244648;
            c[2] = 1.958186055840383226;
            c[3] = -5.202780326728863373;
            c[4] = 13.957913586524019271;
            c[5] = -28.105523286031779624;
            c[6] = 28.625326490479621820;
        }
    }
    else {
        flag = 3;
        c[0] = 0.99995407648636290;
        c[1] = -0.99654366353702101;
        c[2] = 1.89259130089973960;
        c[3] = -4.2080899534548178;
        c[4] = 6.1600151655902185;
    }
    
    // Compute K0 using the different coefficients
    sol = 0;
    if (flag == 1)  {
        t1 = 0;
        for (int j=1; j<=9;j++) {
            t1 += c[j-1]*pow(x,j);
        }
        sol = -EULER-log(x)+t1;
    }
    else if (flag == 2) {
        t1 = 0;
        for (int j=1; j<=7; j++) {
            t1 += c[j-1]*pow(x,-j);
        }
        sol = exp(-x)*t1;
    }
    else if (flag == 3) {
        t1 = 0;
        for (int j=1; j<=5; j++) {
            t1 += c[j-1]*pow(x,-j);
        }
        sol = exp(-x)*t1;
    }
    return sol;
}

/* -----------------------------------------------------------------------/
 * Computes K0(x)
 *-----------------------------------------------------------------------*/
double K0x(double x) {
    int flag = 0;
    double sol=0.,t1,t2,a[10],b[10],c[10];
    // Assign coefficients depending on regime
    if (x < 1.2) {                                                // Regime 1
        flag = 1;
        if (x<.4) {
            a[0] = 0;
            a[1] = 0.249999999997972328;
            a[2] = 0.156249989302374125e-1;
            a[3] = 0.433979473302476069e-3;
            a[4] = 0.651537399996306114e-5;
            
            b[0] = -0.577215664901532810;
            b[1] = 0.105696083784078902;
            b[2] = 0.144185085968622513e-1;
            b[3] = 0.545290428467450103e-3;
            b[4] = 0.104710633532582440e-4;
        }
        else if(x>=.4 && x<.8) {
            a[0] = 0;
            a[1] = 0.249999962288190278;
            a[2] = 0.156242940245049724e-1;
            a[3] = 0.431814195263372214e-3;
            a[4] = 0.551040374186648168e-5;
            b[0] = -0.577215664604050699;
            b[1] = 0.105696186479774189;
            b[2] = 0.144196892856427428e-1;
            b[3] = 0.547087493571828438e-3;
            b[4] = 0.101243380076289042e-4;
        }
        else if (x>=.8 && x<1.2) {
            a[0] = 0;
            a[1] = 0.249998524555063542;
            a[2] = 0.156154220048914215e-1;
            a[3] = 0.423254625846808330e-3;
            a[4] = 0.440999106867886778e-5;
            b[0] = -0.577215630454347469;
            b[1] = 0.105699310614512978;
            b[2] = 0.144295681817805119e-1;
            b[3] = 0.549027302611090957e-3;
            b[4] = 0.898933733297033671e-5;
        }
    }
    else if(x>=1.2 && x<2.6) {                                    // Regime 2
        flag = 2;
        if(x>=1.2 && x<1.4) {
            c[0] = 1.253232926875344025;
            c[1] = -0.155560612591199364;
            c[2] = 0.810366716784953166e-1;
            c[3] = -0.624600233829930736e-1;
            c[4] = 0.483291368907167268e-1;
            c[5] = -0.305853541993908992e-1;
            c[6] = 0.138610454043163412e-1;
            c[7] = -0.389585649536839281e-2;
            c[8] = 0.505146011171134887e-3;
        }
        else if (x>=1.4 && x<1.8) {
            c[0] = 1.253277559124593492;
            c[1] = -0.156066223030992628;
            c[2] = 0.835493060941808475e-1;
            c[3] = -0.696144020365976462e-1;
            c[4] = 0.610951587646532751e-1;
            c[5] = -0.452026870794575869e-1;
            c[6] = 0.243490277791697103e-1;
            c[7] = -0.820693561079057986e-2;
            c[8] = 0.128235623414831454e-2;
        }
        else if (x>=1.8 && x<2.6) {
            c[0] =  1.253304213144263428;
            c[1] = -0.156449241585950637;
            c[2] = 0.859715256343087776e-1;
            c[3] = -0.784201122672813697e-1;
            c[4] = 0.812230980744864994e-1;
            c[5] = -0.748235981352332240e-1;
            c[6] = 0.517530228128731266e-1;
            c[7] = -0.227767334962265870e-1;
            c[8] = 0.468985595082991627e-2;
        }
    }
    else if( x>=2.6 && x<20) {                                    // Regime 3
        flag = 3;
        if(x>=2.6 && x<2.95) {
            c[0] = 1.253297472796261969;
            c[1] = -0.156306710757111439;
            c[2] = 0.846870498242762999e-1;
            c[3] = -0.719867369140086215e-1;
            c[4] = 0.617221499706721442e-1;
            c[5] = -0.385095667287882587e-1;
            c[6] = 0.118684666082804794e-1;
        }
        else if(x>=2.95 && x<3.4) {
            c[0] = 1.253304599116615050;
            c[1] = -0.156432530217884206;
            c[2] = 0.856136508401949559e-1;
            c[3] = -0.756301790284986163e-1;
            c[4] = 0.697894748936783983e-1;
            c[5] = -0.480467809195150689e-1;
            c[6] = 0.165714811352500356e-1;
        }
        else if(x>=3.4 && x<4) {
            c[0] = 1.253309183387182719;
            c[1] = -0.156525642159723134;
            c[2] = 0.864027477437476912e-1;
            c[3] = -0.792017389968658189e-1;
            c[4] = 0.788951702813102591e-1;
            c[5] = -0.604453451871154993e-1;
            c[6] = 0.236154981436937982e-1;
        }
        else if(x>=4 && x<5) {
            c[0] = 1.253312047872078063;
            c[1] = -0.156594274784777751;
            c[2] = 0.870894238106930789e-1;
            c[3] = -0.828739247650115560e-1;
            c[4] = 0.899658041075270910e-1;
            c[5] = -0.782842075521491145e-1;
            c[6] = 0.356185003501613255e-1;
        }
        else if(x>=5 && x<7) {
            c[0] = 1.253313567757878510;
            c[1] = -0.156639526651225415;
            c[2] = 0.876531868984940647e-1;
            c[3] = -0.866359668762953155e-1;
            c[4] = 0.104148107720541338;
            c[5] = -0.106921404239270243;
            c[6] = 0.598140946444428823e-1;
        }
        else if(x>=7 && x<20)  {
            c[0] = 1.253314071554737510;
            c[1] = -0.156659953990195415;
            c[2] = 0.880013781612836135e-1;
            c[3] = -0.898309536054315380e-1;
            c[4] = 0.120797567108349399;
            c[5] = -0.153642802734186392;
            c[6] = 0.114964691525387807;
        }
    }
    else if ( x>=20)  {                                           // Regime 4
        flag = 4;
        {
            c[0] = 1.253306090040425986;
            c[1] = -0.156144105555841567;
            c[2] = 0.0766692867495066413;
        };
    }
    
    // Compute K0 using the different coefficients
    sol = 0;
    if (flag==1)  {
        t1 = 1;t2 = 0;
        for (int j=0; j<=4;j++) {
            t1 += a[j]*pow(x,2*j);
            t2 += b[j]*pow(x,2*j);
        }
        sol = -log(x/2)*t1+t2;
    }
    else if(flag==2) {
        t1 = 0;
        for (int j=1;j<=9;j++) {
            t1 += c[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    }
    else if(flag==3)  {
        t1 = 0;
        for (int j=1;j<=7;j++) {
            t1 += c[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    }
    else if(flag==4) {
        t1 = 0;
        for(int j=1;j<=3;j++) {
            t1 += c[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    }
    return sol;
}

/* -----------------------------------------------------------------------/
 * Computes K1(x)
 *-----------------------------------------------------------------------*/
double K1x(double x) {
    int flag = 0;
    double sol=0.,t1,t2,a[9],b[5];
    // Assign coefficients depending on regime
    if (x < 1.35) {                                              // Regime 1
        flag = 1;
        if (x < 0.6) {
            a[0] = 0.624999999827751216e-1;
            a[1] = 0.260416471709282662e-2;
            a[2] = 0.542273152608498195e-4;
            a[3] = 0.628100206485585732e-6;
            b[0] = -0.386078324507536556e-1;
            b[1] = 0.420490210080776135e-1;
            b[2] = 0.283711681473530068e-2;
            b[3] = 0.749701403374965313e-4;
            b[4] = 0.111267057227150925e-5;
        }
        else if (x >= 0.6 && x < 1.0) {
            a[0] = 0.624999716674553256e-1;
            a[1] = 0.260387851167488181e-2;
            a[2] = 0.537538347972113395e-4;
            a[3] = 0.514893893150713137e-6;
            b[0] = -0.386078320330859727e-1;
            b[1] = 0.420490893511887388e-1;
            b[2] = 0.283750854526189772e-2;
            b[3] = 0.752134519997185860e-4;
            b[4] = 0.103379588289132252e-5;
        }
        else if (x >= 1.0 && x < 1.35) {
            a[0] = 0.624995833249527517e-1;
            a[1] = 0.260222570913041718e-2;
            a[2] = 0.526754169909213450e-4;
            a[3] = 0.423597603298702830e-6;
            b[0] = -0.386078188162595442e-1;
            b[1] = 0.420498624574444215e-1;
            b[2] = 0.283903490710222872e-2;
            b[3] = 0.752402804578363536e-4;
            b[4] = 0.919880464613676100e-6;
        }
    }
    else if (x >= 1.35 && x < 2.4) {                            // Regime 2
        flag = 2;
        if (x >= 1.35 && x < 1.75) {            // OBS a[] = c[]
            a[0] = 1.253366888334759188;
            a[1] = 0.469159498236308862;
            a[2] = -0.140715876738459817;
            a[3] = 0.996780601560284940e-1;
            a[4] = -0.809047585339955331e-1;
            a[5] = 0.568213378372859195e-1;
            a[6] = -0.293294620626694615e-1;
            a[7] = 0.950976052894363968e-2;
            a[8] = -0.143200996318142944e-2;
        }
        else if (x >= 1-75 && x < 2.4) {
            a[0] = 1.253329635064877431;
            a[1] = 0.469674162376275034;
            a[2] = -0.143842833628629803;
            a[3] = 0.110592069450223066;
            a[4] = -0.104840655041398431;
            a[5] = 0.905979398700767529e-1;
            a[6] = -0.592768276428569641e-1;
            a[7] = 0.247612929675964516e-1;
            a[8] = -0.484732268897594967e-2;
        }
    }
    else if (x >= 2.4 && x < 20) {                              // Regime 3
        flag = 3;
        if (x >= 2.4 && x < 2.8) {
            a[0] = 1.253341744761548085;
            a[1] = 0.469437742992200264;
            a[2] = -0.141869300030892209;
            a[3] = 0.101413106961941722;
            a[4] = -0.789485272879443500e-1;
            a[5] = 0.456357811385575880e-1;
            a[6] = -0.131067895325653096e-1;
            a[7] = 0.0;
            a[8] = 0.0;
        }
        else if (x >= 2.8 && x < 3.4) {
            a[0] = 1.253327547743503557;
            a[1] = 0.469675980847858834;
            a[2] = -0.143538089591062772;
            a[3] = 0.107658822186875543;
            a[4] = -0.921212650220200726e-1;
            a[5] = 0.604799548690602384e-1;
            a[6] = -0.200892075080795317e-1;
            a[7] = 0.0;
            a[8] = 0.0;
        }
        else if (x >= 3.4 && x<4.25) {
            a[0] = 1.253319533869943733;
            a[1] = 0.469837893697954236;
            a[2] = -0.144904636983581356;
            a[3] = 0.113826114104207944;
            a[4] = -0.107818399062207938;
            a[5] = 0.818437354026608345e-1;
            a[6] = -0.322357084123886584e-1;
            a[7] = 0.0;
            a[8] = 0.0;
        }
        else if (x >= 4.25 && x < 6) {
            a[0] = 1.253315628917543631;
            a[1] = 0.469937076179779401;
            a[2] = -0.145958929839169666;
            a[3] = 0.119829920007366109;
            a[4] = -0.127136242718770634;
            a[5] = 0.115141771721845979;
            a[6] = -0.562551854548565506e-1;
            a[7] = 0.0;
            a[8] = 0.0;
        }
        else if (x >= 6  && x < 20) {
            a[0] = 1.253314292367396961;
            a[1] = 0.469983769466232163;
            a[2] = -0.146645512928429718;
            a[3] = 0.125271244559426978;
            a[4] = -0.151655568348607727;
            a[5] = 0.174703946592187625;
            a[6] = -0.117175845236100321;
            a[7] = 0.0;
            a[8] = 0.0;
        }
    }
    else if (x >= 20) {                                         // Regime 4
        flag = 4;
        a[0] = 1.253325590451110162;
        a[1] = 0.469253888870098903;
        a[2] = -0.130663020947967846;
        a[3] = 0.0;
        a[4] = 0.0;
        a[5] = 0.0;
        a[6] = 0.0;
        a[7] = 0.0;
        a[8] = 0.0;
    }
    
    // Compute K0 using the different coefficients
    if (flag == 1) {
        t1 = 0; //for a coeff
        t2 = b[0];
        for (int j=1; j <= 4; j++) {
            t1 += a[j-1]*pow(x,2*j);
            t2 += b[j]*pow(x,2*j);
        }
        t2 *= x;
        sol = 1/x + x*log(x/2)*(0.5+t1)-t2;
    }
    else if (flag == 2) {
        t1 = 0; // obs c=a
        for (int j = 1; j <= 9; j++) {
            t1 += a[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    }
    else if (flag == 3) {
        t1 = 0;
        for (int j = 1; j <= 7; j++) {
            t1 += a[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    } else if (flag == 4) {
        t1 = 0;
        for (int j = 1; j <= 3; j++) {
            t1 += a[j-1]*pow(x,-j);
        }
        sol = sqrt(x)*exp(-x)*t1;
    }
    return sol;
}