helpFunction()
{
   echo ""
   echo "Como se usa: $0 nombreConfig , donde nombreConfig puede ser:"
   echo -e "\tcons1"
   echo -e "\tcons2"
   echo -e "\tcomp1"
   echo -e "\tcomp2"
   echo -e "\tbs1"
   echo -e "\tbs2"
   echo -e "\tcompleta"

   exit 1 # Exit script after printing help
}

case $1 in
    cons1)
        cp cfg/broker1.config cfg/broker.config
        echo -e "Seteando config de Consolidacion de Particiones Parte 1"
        ;;
    cons2)
        cp cfg/broker2.config cfg/broker.config
        echo -e "Seteando config de Consolidacion de Particiones Parte 2"
        ;;
    comp1)
        cp cfg/broker3.config cfg/broker.config
        echo -e "Seteando config de Compactacion de Particiones Parte 1"
        ;;
    comp2)
        cp cfg/broker4.config cfg/broker.config
        echo -e "Seteando config de Compactación de Particiones Parte 2"
        ;;
    bs1)
        cp cfg/broker5.config cfg/broker.config
        echo -e "Seteando config de Buddy System Parte 1"
        ;;
    bs2)
        cp cfg/broker6.config cfg/broker.config
        echo -e "Seteando config de Buddy System Parte 2"
        ;;
    completa)
        cp cfg/broker2.config cfg/broker.config
        echo -e "Seteando config de Prueba Integradora"
        ;;
    ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
esac

# Print helpFunction in case parameters are empty
if [ -z "$1" ]
then
   echo "Falta el nombre de lo que queres cambiar";
   helpFunction
fi