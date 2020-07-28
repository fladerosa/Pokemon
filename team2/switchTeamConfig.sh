helpFunction()
{
   echo ""
   echo "Como se usa: $0 nombreConfig , donde nombreConfig puede ser:"
   echo -e "\trr"
   echo -e "\tsjf_cd"

   exit 1 # Exit script after printing help
}

if [ -n "$1" ]
then
    truncate -s 0 ./cfg/team.config
    echo -e "IP_TEAM=127.0.0.2\nPUERTO_TEAM=6012\nPOSICIONES_ENTRENADORES=[2|3,6|5,9|9,9|2,2|9]\nPOKEMON_ENTRENADORES=[]\nOBJETIVOS_ENTRENADORES=[Vaporeon,Jolteon,Flareon,Umbreon,Espeon]\nTIEMPO_RECONEXION=30\nRETARDO_CICLO_CPU=5\nESTIMACION_INICIAL=5\nIP_BROKER=127.0.0.1 \nPUERTO_BROKER=6009\nLOG_FILE=./cfg/logObligatorio.log\nLOG_FILE_OPTIONAL=./cfg/logOptional.log\nCONNECTION_TIME=10 " >>  ./cfg/team.config
fi

case $1 in
    rr)
        echo -e "Seteando config de Algoritmo Round Robin"
        echo -e "ALGORITMO_PLANIFICACION=RR\nQUANTUM=1\nALPHA=0"  >>  ./cfg/team.config
        ;;
    sjf_cd)
        echo -e "Seteando config de Algoritmo SJF con desalojo"
        echo -e "ALGORITMO_PLANIFICACION=SJF-CD\nQUANTUM=0\nALPHA=0.5" >>  ./cfg/team.config
        ;;
    ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
esac

# Print helpFunction in case parameters are empty
if [ -z "$1" ]
then
   echo "Falta el nombre de lo que queres cambiar";
   helpFunction
fi
