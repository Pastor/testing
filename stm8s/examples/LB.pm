package LB;
require 5.004;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(TYPE_VG_LOGIN TYPE_USER_LOGIN TYPE_TEL_STAFF TYPE_STAFF TYPE_FIO TYPE_AGRM_NUM TYPE_KOD_1C TYPE_EMAIL TYPE_VG_ID TYPE_UID TYPE_AGRM_CODE TYPE_AGRM_ID TYPE_AGRM_ADDON TYPE_ORDER_ID TYPE_VG_LOGIN_SUFFIX TYPE_AGRM_NUM_TYPE TYPE_OPER_ID);

use POSIX qw(mktime strftime floor);
#use SOAP::Lite +trace    =>  qw(debug);
use SOAP::Lite;
use Data::Dumper;
use Compress::Zlib;

use constant TYPE_VG_LOGIN                      => 0;   # vgroups.login
use constant TYPE_USER_LOGIN            => 1;   # accounts.login
use constant TYPE_TEL_STAFF                     => 2;   # tel_staff.phone_number ( ts join vgroups v on ts.vg_id = v.vg_id )
use constant TYPE_STAFF                         => 3;   # staff.segment ( ts join vgroups v on ts.vg_id = v.vg_id )
use constant TYPE_FIO                           => 4;   # accounts.name
use constant TYPE_AGRM_NUM                      => 5;   # agreements.number
use constant TYPE_KOD_1C                        => 6;   # 
use constant TYPE_AGRM_CODE                     => 6;   # agreements.code
use constant TYPE_EMAIL                         => 7;   # accounts.email
use constant TYPE_ORDER                         => 8;   # 
use constant TYPE_VG_ID                         => 9;   # vgroups.vg_id
use constant TYPE_UID                           => 10;  # accounts.uid
use constant TYPE_AGRM_ID                       => 11;  # agreements.agrm_id
use constant TYPE_SBRF_RYAZAN           => 12;  # agreements.number, agreements_addons_vals.str_value
use constant TYPE_AGRM_ADDON            => 13;  # options, name = old_agreement
use constant TYPE_ORDER_ID                      => 14;  # orders.order_id
use constant TYPE_VG_LOGIN_SUFFIX       => 15;  # part of vgroups.login, using options.name = extern_payment_regexp_suffix
use constant TYPE_AGRM_NUM_TYPE         => 16;  #Saratov's agreement number with type of account
use constant TYPE_OPER_ID                       => 17;  #agreements.oper_id

my $currid = 0;

=head1 NAME

LB - LANBilling 1.9 API Interface

=head1 SYNOPSIS

use LB;

my $pay = new LB(host=>'localhost', port=>34012, user=>'manager', pass=>'manager_pass');

$pay->connect() || die("LB connection failed");

my $ret = $pay->check(number=>"777777", type=>TYPE_AGRM_NUM);

print "user found\n" if defined($ret) && $ret eq '0';

$pay->disconnect;

=head1 DESCRIPTION

This module provides a simple interface to LANBilling 1.9 payment API.

=cut


my $VERSION = '2.0.1';

sub version { $VERSION; }

sub new {
    my ($class, %args) = @_;
    my $self = {};
    $self->{_CONNFD} = undef;
    $self->{_CONCFG} = undef;
    $self->{_CONNECTED} = 0;
    $self->{_PROTO} = $args{'proto'} || 'http';
    $self->{_HOST} = $args{'host'} || 'localhost';
    $self->{_PORT} = $args{'port'} || 34012;
    $self->{_USER} = $args{'user'} || 'unknown_user';
    $self->{_PASS} = $args{'pass'} || 'unknown_pass';
    bless $self, ref $class || $class;
    return $self;
}

sub DESTROY { }

sub connected 
{
    my ($self, $connected) = @_;
    if (defined($connected)) 
    {
        $self->{_CONNECTED} = $connected;
    }
    return $self->{_CONNECTED};
}


sub connfd 
{
    my ($self, $connfd) = @_;
    if ($connfd) 
    {
        $self->{_CONNFD} = $connfd;
    }
    return $self->{_CONNFD};
}

sub concfg 
{
    my ($self, $concfg) = @_;
    if ($concfg) 
    {
        $self->{_CONCFG} = $concfg;
    }
    return $self->{_CONCFG};
}

sub proto
{
    my ($self, $proto) = @_;
    if ($proto) 
    {
        $self->{_PROTO} = $proto;
    }
    return $self->{_PROTO};
}

sub host 
{
    my ($self, $host) = @_;
    if ($host) 
    {
        $self->{_HOST} = $host;
    }
    return $self->{_HOST};
}

sub port 
{
    my ($self, $port) = @_;
    if ($port) 
    {
        $self->{_PORT} = $port;
    }
    return $self->{_PORT};
}

sub user 
{
    my ($self, $user) = @_;
    if ($user) 
    {
        $self->{_USER} = $user;
    }
    return $self->{_USER};
}

sub pass 
{
    my ($self, $pass) = @_;
    if ($pass) 
    {
        $self->{_PASS} = $pass;
    }
    return $self->{_PASS};
}

=head1 CONSTANTS

        TYPE_VG_LOGIN                   - Vgroup login
        TYPE_USER_LOGIN                 - User login
        TYPE_TEL_STAFF                  - Phone number assigned to vgroup
        TYPE_STAFF                      - IP address assigned to vgroup
        TYPE_FIO                        - User name
        TYPE_AGRM_NUM                   - Contract number
        TYPE_AGRM_CODE                  - Paycode number
        TYPE_KOD_1C                     - Synonym for TYPE_AGRM_CODE (1.8 compatibility)
        TYPE_EMAIL                      - User E-Mail
        TYPE_ORDER                      - Order ID (payment against an invoice)
        TYPE_VG_ID                      - Vgroup ID
        TYPE_UID                        - User ID
        TYPE_AGRM_ID                    - Contract ID
    TYPE_SBRF_RYAZAN            - agreements_addons_vals.str_value
    TYPE_AGRM_ADDON                     - options, name = old_agreement
        TYPE_ORDER_ID                   - orders.order_id
        TYPE_VG_LOGIN_SUFFIX    - part of vgroups.login, using options.name = extern_payment_regexp_suffix
        TYPE_OPER_ID                    - agreements.oper_id
                
=cut

=head1 METHODS

        new([host=>"host", port=>"tcp port"], user=>"manager login", pass=>"manager password")
                Constructor for LB connector. user and pass are required for LBcore connection, host and port are optional (default: localhost:34012)
        connect()
                Connect to LBcore server
        check(number=>"number", type=>"type", amount=>"amount" [,fetch_balance=>"yes/no", fetch_account=>"one of TYPE_* constants"])
                Check if user exists
                number  - user ID
                type    - ID type (TYPE_* constant)
                Returns error code, current balance and user attribute according to TYPE_* constant (optional)
        payment(number=>"number", type=>"type", amount=>"amount", receipt=>"receipt", date=>"date(unix timestamp)", additional=>"additional")
                Perform payment
                amount  - payment amount xxxx.yy
                receipt - unique payment ID in payment system
                date    - unix timestamp for payment moment
                Returns error code, auth_code (payment id in billing system)
        cancel(receipt=>"receipt", date=>unixtime)
                Cancel payment
                Returns error code, auth_code (payment id in billing system)
        status(receipt=>"receipt" [,type=>"type"])
                Get payment status
                Returns error code, auth_code (payment id in billing system) [, user attribute, payment amount, payment date, cancel date]
        getpayment(receipt=>"receipt")
                Get payment
                Returns payment if exists;
        disconnect
                Close connection

=cut

sub connect 
{
    my ($self) = @_;
    my $conn = SOAP::Lite->uri('urn:api3')->proxy($self->proto."://".$self->host.":".$self->port, keep_alive => 1, timeout => 90, options => {compress_threshold => 100000});
    return undef if !$conn;
    $conn->proxy->http_request->headers->header('User-Agent' => 'LBpm/'.$self->version);
    my $ret;
    eval { $ret = $conn->Login(
            SOAP::Data->new(name => 'login', value => $self->user, type => 'string'),
                SOAP::Data->new(name => 'pass', value => $self->pass, type => 'string')
            );
        };
    if(!defined($ret) || defined($ret->fault)) 
    {
        return undef;
    }

    my $cookie = $conn->proxy->http_response->headers->header('Set-Cookie');
    my $session = $1 if defined($cookie) && $cookie =~ /sessnum=(\w+);/;
    $conn->proxy->http_request->headers->header('Cookie' => "sessnum=$session;") if defined $session;
    $self->connfd($conn);
    $self->concfg($ret);
    return $conn;
}

sub check
{
    my ($self,%params)  = @_;
    my $fetch_balance = 0;
    my $fetch_param = 0;
    my $fetch_operid = 0;
    if ( defined($params{'operid'}) )
    {
        $fetch_operid = $params{'operid'};
    }
    $fetch_balance = 1 if(defined($params{'fetch_balance'}) && ($params{'fetch_balance'} =~ /yes/i));
    $fetch_param = $params{'fetch_param'} if(defined($params{'fetch_param'}) && ($params{'fetch_param'} =~ /\d+/));
    my $ret;
    eval 
    {
        $ret = $self->connfd()->getExternAccount(
        SOAP::Data->new(name => 'id', value => $params{'type'}, type => 'long'), 
        SOAP::Data->new(name => 'str', value => $params{'number'}, type => 'string'), 
        SOAP::Data->new(value => $params{'amount'}, type => 'float'),
        SOAP::Data->new(name => 'operid', value => $fetch_operid, type => 'long')
    );
    };

    if(!defined($ret)) 
    {
        return 11;
    } 
    elsif(defined($ret->fault)) 
    {
        if($ret->fault->{'detail'} =~ /.*not found.*/i) 
        {
            return 2;
        }
        elsif($ret->fault->{'detail'} =~ /.*already exists \(record_id = (\d+)\).*/i) 
        {
            return 12;
        } 
        else 
        {
            return 11;
        }
    } 
    elsif(!defined($ret->result)) 
    {
        return 11;
    }
    if($fetch_balance || $fetch_param) 
    {
        my $agrm = undef;
        if($ret->result->{'agreements'} =~ /ARRAY/) 
        {
            if  (   $params{'type'} == TYPE_AGRM_NUM 
                ||  $params{'type'} == TYPE_AGRM_CODE 
                ||  $params{'type'} == TYPE_AGRM_ID
                ) 
            {
                foreach my $a1($ret->result->{'agreements'}) 
                {
                    foreach my $a2(@$a1) 
                    {
                        if  (       (($params{'type'} == TYPE_AGRM_NUM)     && ($a2->{'number'} eq $params{'number'})   )
                                    ||  (($params{'type'} == TYPE_AGRM_CODE)    && ($a2->{'code'} eq $params{'number'})     )
                                    ||  (($params{'type'} == TYPE_AGRM_ID)      && ($a2->{'agrmid'} eq $params{'number'})   )
                            ) 
                        {
                            $agrm = $a2;
                            last;
                        }
                        last if defined($agrm);
                    }
                }
            } 
            else 
            {
                $agrm = $ret->result->{'agreements'}[0];
            }
        } 
        else 
        {
            $agrm = $ret->result->{'agreements'};
        }

        if($fetch_param) 
        {
            if      (($fetch_param == TYPE_AGRM_NUM) && defined($agrm))     {       $acc = $agrm->{'number'};                       } 
            elsif   (($fetch_param == TYPE_AGRM_ID) && defined($agrm))      {       $acc = $agrm->{'agrmid'};                           } 
            elsif   (($fetch_param == TYPE_AGRM_CODE) && defined($agrm))    {       $acc = $agrm->{'code'};                             } 
            elsif   ($fetch_param == TYPE_FIO)                              {       $acc = $ret->result->{'account'}->{'name'};         } 
            elsif   ($fetch_param == TYPE_USER_LOGIN)                       {       $acc = $ret->result->{'account'}->{'login'};    } 
            elsif   ($fetch_param == TYPE_EMAIL)                            {       $acc = $ret->result->{'account'}->{'email'};    }
            elsif   ($fetch_param == TYPE_UID)                              {       $acc = $ret->result->{'account'}->{'uid'};              }
            elsif   ($fetch_param == TYPE_OPER_ID)                          {       $acc = $agrm->{'operid'}                        }
        }
        my $balance = '0.00';
        if(defined($agrm)) 
        {
            $balance = sprintf("%.2f", $agrm->{'balance'}); 
        }

        # проверка для Саратова fullname
        if (defined($params{'fullname'}))
        {
            if ($params{'fullname'} eq $ret->result->{'account'}->{'abonentsurname'})
            {
                return (0, $balance, defined($acc) ? $acc : 'unknown');
            }
            else
            {
                return 999;
            }
        }
        else
        {
            return (0, $balance, defined($acc) ? $acc : 'unknown');
        }
    } 
    else 
    {
        return 0;
    }
}

sub payment
{
    my ($self, %params)  = @_;
    my $classid = 0;
    if( defined( $params{'classid'} ) )
    {
        $classid = $params{'classid'};
    }
    my $fetch_operid = 0;
    if ( defined($params{'operid'}) )
    {
        $fetch_operid = $params{'operid'};
    }
    my $pay = {
        modperson => SOAP::Data->new(value => 0, type => 'long'),
        currid => SOAP::Data->new(value => $currid, type => 'long'),
        amount => SOAP::Data->new(value => $params{'amount'}, type => 'float'),
        paydate => SOAP::Data->new(value => strftime("%Y-%m-%d %H:%M:%S", localtime($params{'date'})), type => 'string'),
        receipt => SOAP::Data->new(value => $params{'receipt'}, type => 'string'),
        comment => SOAP::Data->new(value => $params{'additional'}, type => 'string'),
        classid => SOAP::Data->new(value => $classid, type => 'long')
    };
    my $ret;
    eval {
        $ret = $self->connfd()->ExternPayment(
            SOAP::Data->new(name => 'id', value => $params{'type'}, type => 'long'),
            SOAP::Data->new(name => 'str', value => $params{'number'}, type => 'string'),
            SOAP::Data->new(name => 'val', value => $pay),
            SOAP::Data->new(name => 'operid', value => $fetch_operid, type => 'long'),
            SOAP::Data->new(name => 'notexists', value => 1, type => 'long')
        );
    };

    return ( 11, 0 ) if(!defined($ret));
    if(defined($ret->fault)) {
        if($ret->fault->{'detail'} =~ /.*not found.*/i) 
        {
            return ( 2, 0 );
        } 
        elsif($ret->fault->{'detail'} =~ /.*is cancelled \(record_id = (\d+)\).*/i) 
        {
            return ( 7, $1 );
        } elsif($ret->fault->{'detail'} =~ /.*already exists \(record_id = (\d+)\).*/i) 
        {
            my $retpay;
            my $filter;
            my $mgrid;
            $mgrid = $self->concfg()->result->{'manager'}->{'personid'};
            $filter = { 
                receipt => SOAP::Data->new(value => $params{'receipt'}, type => 'string'),
                mgrid => SOAP::Data->new(value => $mgrid, type => 'long')
            };

            $retpay = $self->connfd()->getPayments(SOAP::Data->new(name => 'flt', value=> $filter));

            if( defined( $retpay->result ) && defined( $retpay->result->{'amountcurr'} ) )
            {
                return ( 12, $1,  $retpay->result->{'amountcurr'});
            }
            return ( 12, $1, 0 );
        } 
        else 
        {
            return ( 11, 0 );
        }
    }
    return ( 11, 0 ) if(!defined($ret->result) || !$ret->result);
    return ( 0, $ret->result );
}


sub getprepayment
{
    my ($self, %params)  = @_;
    
    my $soapFilter = {
        recordid => SOAP::Data->new(value => $params{'record_id'}, type => 'long')
    };
    my $ret;
    eval {
        $ret = $self->connfd()->getPrePayments(
            SOAP::Data->new(name => 'flt', value => $soapFilter)
        );
    };
    return $ret;
}


sub prepayment
{
    my ($self, %params)  = @_;

    my $ret;
    eval {
        $ret = $self->connfd()->confirmPrePayment(
            SOAP::Data->new(name => 'recordid', value => $params{'record_id'}, type => 'long'),
            SOAP::Data->new(name => 'receipt', value => $params{'receipt'}, type => 'string'),
            SOAP::Data->new(name => 'paydate', value => strftime("%Y-%m-%d %H:%M:%S", localtime($params{'pay_date'})), type => 'string'),
            SOAP::Data->new(name => 'amount', value => $params{'amount'}, type => 'double')
#           SOAP::Data->new(name => 'classid', value => 0, type => 'sting'),
        );
    };

    return ( 11, 0 ) if(!defined($ret));
    if(defined($ret->fault)) {
        if($ret->fault->{'detail'} =~ /.*not found.*/i) 
        {
            return ( 2, 0 );
        } 
        elsif($ret->fault->{'detail'} =~ /.*is cancelled \(record_id = (\d+)\).*/i) 
        {
            return ( 7, $1 );
        } elsif($ret->fault->{'detail'} =~ /.*already exists \(record_id = (\d+)\).*/i) 
        {
            my $retpay;
            my $filter;
            my $mgrid;
            $mgrid = $self->concfg()->result->{'manager'}->{'personid'};
            $filter = { 
                receipt => SOAP::Data->new(value => $params{'receipt'}, type => 'string'),
                mgrid => SOAP::Data->new(value => $mgrid, type => 'long')
            };

            $retpay = $self->connfd()->getPayments(SOAP::Data->new(name => 'flt', value=> $filter));

            if( defined( $retpay->result ) && defined( $retpay->result->{'amountcurr'} ) )
            {
                return ( 12, $1,  $retpay->result->{'amountcurr'});
            }
            return ( 12, $1, 0 );
        } 
        else 
        {
            return ( 11, 0 );
        }
    }
    return ( 11, 0 ) if(!defined($ret->result) || !$ret->result);
    return ( 0, $ret->result );
}

sub cancel_prepayment
{
    my ($self, %params)  = @_;
    
    my $ret;
    eval {
        $ret = $self->connfd()->cancelPrePayment(
            SOAP::Data->new(name => 'recordid', value => $params{'record_id'}, type => 'long')
        );
    };
    return $ret;
}



sub cancel
{
    my ($self, %params)  = @_;
    my $ret;
    eval {
        if(defined($params{'date'})) 
        {
            $ret = $self->connfd()->ExternCancelPayment(
                SOAP::Data->new(name => 'receipt', value => $params{'receipt'}, type => 'string'), 
                    SOAP::Data->new(name => 'canceldate', value => strftime("%Y-%m-%d %H:%M:%S", localtime($params{'date'})), type => 'string'),
                    SOAP::Data->new(name => 'notexists', value => 1, type => 'long')
            );
        } else {
            $ret = $self->connfd()->ExternCancelPayment(
                SOAP::Data->new(name => 'receipt', value => $params{'receipt'}, type => 'string'),
                    SOAP::Data->new(name => 'notexists', value => 1, type => 'long')
            );
        }
    };
    return ( 11, 0 ) if(!defined($ret));
    if(defined($ret->fault)) {
        if($ret->fault->{'detail'} =~ /.*not found.*/i) {
            return ( 6, 0 );
        } elsif($ret->fault->{'detail'} =~ /.*cannot be cancelled.*/i) {
            return ( 9, 0 );
        } elsif($ret->fault->{'detail'} =~ /.*already cancelled \(record_id = (\d+)\).*/i) {
            return ( 7, $1 );
        } else {
            return ( 11, 0 );
        }
    }
    return ( 11, 0 ) if(!defined($ret->result) || !$ret->result);
    return ( 0, $ret->result );
}


sub status
{
    my ($self, %params)  = @_;
    my $ret;
    eval { 
        $ret = $self->connfd()->ExternCheckPayment(
            SOAP::Data->new(
                    name => 'receipt', 
                    value => $params{'receipt'}, 
                    type => 'string')
                ); 
    };
    return ( 11, 0 ) if(!defined($ret));
    if(defined($ret->fault)) 
    {
        if($ret->fault->{'detail'} =~ /.*not found.*/i) 
        {
            return ( 6, 0 );
        } 
        else 
        {
            return ( 11, 0 );
        }
    }
    return ( 11, 0 ) if(!defined($ret->result) || !defined($ret->result->{'pay'}));
    if (defined($params{'type'})) 
    {
        my $ret2;
        my $agrm_id = $ret->result->{'pay'}->{'agrmid'};
        eval { 
            $ret2 = $self->connfd()->getExternAccount(
                SOAP::Data->new(name => 'id', value => TYPE_AGRM_ID, type => 'long'), 
                SOAP::Data->new(name => 'str', value => $agrm_id, type => 'string')
            ); 
        };
        if(defined($ret2) && defined($ret2->result) && !defined($ret2->fault)) 
        {
            my $acc;
            my $agrm;
            if($ret2->result->{'agreements'} =~ /ARRAY/) {
                foreach my $a1 ($ret2->result->{'agreements'}) {
                    foreach my $a2 (@$a1) {
                        if($a2->{'agrmid'} eq $agrm_id) {
                            $agrm = $a2;
                            last;
                        }
                    }
                    last if defined($agrm);
                }
            } else {
                $agrm = $ret2->result->{'agreements'};
            }
            if(($params{'type'} == TYPE_AGRM_NUM) && defined($agrm)) {
                $acc = $agrm->{'number'};
            } elsif(($params{'type'} == TYPE_AGRM_ID) && defined($agrm)) {
                $acc = $agrm->{'agrmid'};
            } elsif(($params{'type'} == TYPE_AGRM_CODE) && defined($agrm)) {
                $acc = $agrm->{'code'};
            } elsif($params{'type'} == TYPE_FIO) {
                $acc = $ret2->result->{'account'}->{'name'};
            } elsif($params{'type'} == TYPE_USER_LOGIN) {
                $acc = $ret2->result->{'account'}->{'login'};
            } elsif($params{'type'} == TYPE_EMAIL) {
                $acc = $ret2->result->{'account'}->{'email'};
            } elsif($params{'type'} == TYPE_EMAIL) {
                $acc = $ret2->result->{'account'}->{'email'};
            } elsif($params{'type'} == TYPE_UID) {
                $acc = $ret2->result->{'account'}->{'uid'};
            }
            my $pdate = mktime($6, $5, $4, $3, $2 - 1, $1 - 1900, 0, 0, -1) if $ret->result->{'pay'}->{'paydate'} =~ /(\d{4})-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)/;
            my $cdate = mktime($6, $5, $4, $3, $2 - 1, $1 - 1900, 0, 0, -1) if $ret->result->{'pay'}->{'canceldate'} =~ /(\d{4})-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)/;
            my $ldate = mktime($6, $5, $4, $3, $2 - 1, $1 - 1900, 0, 0, -1) if $ret->result->{'pay'}->{'localdate'} =~ /(\d{4})-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)/;
            my $amount = floor($ret->result->{'pay'}->{'amount'} * 100 + 0.5);
            $amount = (length($amount) < 3 ? '0' : '') . substr($amount, 0, length($amount) - 2) . '.' . substr($amount, length($amount) - 2);
            return ( 
                $ret->result->{'pay'}->{'status'} == 2 ? 7 : 0, 
                $ret->result->{'pay'}->{'recordid'}, 
                defined($acc) ? $acc : 'unknown', 
                $amount, 
                defined($pdate) ? $pdate : 0, 
                defined($cdate) ? $cdate : 0,
                defined($ldate) ? $ldate : 0
            );
        } else {
            return ( $ret->result->{'pay'}->{'status'} == 2 ? 7 : 0, $ret->result->{'pay'}->{'recordid'} );
        }
    } else {
        return ( $ret->result->{'pay'}->{'status'} == 2 ? 7 : 0, $ret->result->{'pay'}->{'recordid'} );
    }
}

sub findagreement {
    my ($self,%params)  = @_;
    my $fetch_operid = 0;
    if ( defined($params{'operid'}) ) {
        $fetch_operid = $params{'operid'};
    }

    my $ret;
    eval {
        $ret = $self->connfd()->getExternAgreement(
            SOAP::Data->new(name => 'id', value => $params{'type'}, type => 'long'), 
            SOAP::Data->new(name => 'str', value => $params{'number'}, type => 'string'), 
            SOAP::Data->new(name => 'operid', value => $fetch_operid, type => 'long')
            );
    };

    return $ret->result;
}

sub getpayment
{
    my ($self, %params)  = @_;
    my $ret;
    eval { 
        $ret = $self->connfd()->ExternCheckPayment(
            SOAP::Data->new(
                name => 'receipt', 
                value => $params{'receipt'}, 
                type => 'string')
            ); 
    };
    return $ret;
}

sub disconnect {
    my ($self) = @_;
    eval { $self->connfd()->Logout(); };
    $self->{_CONNFD} = undef;
    $self->connected(0);
    return 1;
}

sub account {
    my $ret = undef;
    my ($self, %params)  = @_;
    my ($name, $param) = (undef, undef);

    ($name, $param) = ('login', $params{'login'}) if defined $params{'login'};
    ($name, $param) = ('phone', $params{'phone'}) if defined $params{'phone'} and $params{'phone'} =~ /^\d{10,}$/;
    ($name, $param) = ('mobile', $params{'mobile'}) if defined $params{'mobile'} and $params{'mobile'} =~ /^\d{10,}$/;
    ($name, $param) = ('vglogin', $params{'vglogin'}) if defined $params{'vglogin'};
    ($name, $param) = ('agrmnum', $params{'agreement'}) if defined $params{'agreement'};

    return undef unless defined $name;
    return undef unless defined $param;

    my $filter = { 
        #vglogin => SOAP::Data->new(value => $params{'vglogin'}, type => 'string'),
        #login => SOAP::Data->new(value => $params{'login'}, type => 'string'),
        $name => SOAP::Data->new(value => $param, type => 'string'),
        type  => SOAP::Data->new(value => 2, type => 'long'),
    };
    eval {
        $ret = $self->connfd()->getAccounts(SOAP::Data->new(name => 'flt', value => $filter));
    };

    return undef unless defined $ret;

    if ( defined $ret->fault ) {
        return undef;
    }

    my @set = $ret->valueof('//getAccountsResponse/ret');
    
    #print qq|\n|.Dumper(@set).qq|\n|;
    return $set[0] if scalar(@set) == 1 and $name == 'vglogin';
    foreach my $elem (@set) {
      #print qq|\n|.Dumper($elem->{'account'}).qq|\n|;
      return $elem if $elem->{'account'}->{$name} eq $param;
    }
    return undef;
}


sub findaccount {
    my ($self,%params)  = @_;
    my $fetch_operid = 0;
    if ( defined($params{'operid'}) ) {
        $fetch_operid = $params{'operid'};
    }
 
    my $ret;
    eval {
        $ret = $self->connfd()->getExternAccount(
            SOAP::Data->new(name => 'id', value => $params{'type'}, type => 'long'), 
            SOAP::Data->new(name => 'str', value => $params{'number'}, type => 'string'), 
            SOAP::Data->new(name => 'operid', value => $fetch_operid, type => 'long')
            );
    };
    return $ret->result;
}

sub license {
    my ($self) = @_;
	my $ret;
    eval { $ret = $self->connfd()->getLicense(); };
    return $ret->result;
}

sub accounts {
    my ($self, %params)  = @_;
	my $ret;
	my $filter = { 
        type => SOAP::Data->new(value => 1, type => 'long')
        };
	eval {
        $ret = $self->connfd()->getAccounts(SOAP::Data->new(name => 'flt', value => $filter));
	};
	if ( defined($ret->fault) ) {
		return ();
	}
    return $ret->valueof('//getAccountsResponse/ret');
}

sub vgroup {
   my ($self, %params)  = @_;
   my $ret;
   my $filter = { 
     login => SOAP::Data->new(value => $params{'login'}, type => 'string')
   };
   eval {
     $ret = $self->connfd()->getVgroups(SOAP::Data->new(name => 'flt', value => $filter));
   };
   if ( defined($ret->fault) ) {
     return undef;
   }
   my $vgid = $ret->result->{'uid'};
   $filter = { 
     id => SOAP::Data->new(value => $vgid, type => 'long')
   };
   eval {
     $ret = $self->connfd()->getVgroup(SOAP::Data->new(name => 'flt', value => $filter));
   };
   if ( defined($ret->fault) ) {
     return undef;
   }
   return $ret->result;
}


sub vlans {
    my ($self, %params)  = @_;
	my $ret;
	my $filter = { 
        login => SOAP::Data->new(value => $params{'login'}, type => 'string')
    };
	eval {
        $ret = $self->connfd()->getVgroups(SOAP::Data->new(name => 'flt', value => $filter));
	};
	if ( defined($ret->fault) ) {
		return undef;
	}

	my $vgid = $ret->result->{'vgid'};
	$filter = { 
        vgid => SOAP::Data->new(value => $vgid, type => 'long')
    };
	eval {
        $ret = $self->connfd()->getPorts(SOAP::Data->new(name => 'flt', value => $filter));
	};
	if ( defined($ret->fault) ) {
		return undef;
	}
	return $ret->result;
}

sub segments {
    my ($self, %params)  = @_;
	my $ret;
	my $filter = { 
        login => SOAP::Data->new(value => $params{'login'}, type => 'string')
    };
	eval {
        $ret = $self->connfd()->getVgroups(SOAP::Data->new(name => 'flt', value => $filter));
	};
	if ( defined($ret->fault) ) {
		return undef;
	}

	my $record_id = $ret->result->{'recordid'};
	$filter = { 
        recordid => SOAP::Data->new(value => $record_id, type => 'long'),
    };
	eval {
        $ret = $self->connfd()->getSegments(SOAP::Data->new(name => 'flt', value => $filter));
    };
	if ( defined($ret->fault) ) {
		return undef;
	}
	return $ret->result;
}

1;

