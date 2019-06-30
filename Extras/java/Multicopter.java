/*
   Java Multicopter class

   Uses UDP sockets to communicate with MulticopterSim

   Copyright(C) 2019 Simon D.Levy

   MIT License
 */

import java.lang.Thread;
import java.io.*;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;

class Multicopter extends Thread {

    private final int TIMEOUT = 1000;

    public Multicopter(String host, int motorPort, int telemetryPort, int motorCount)
    {
        construct(host, motorPort, telemetryPort, motorCount);
    }

    public Multicopter(String host, int motorPort, int telemetryPort)
    {
        construct(host, motorPort, telemetryPort, 4);
    }

    public Multicopter()
    {
        construct("127.0.0.1", 5000, 5001, 4);
    }

    public void run()
    {
        _running = true;

        int count = 0;

        while (_running) {

            byte [] motorBytes = doublesToBytes(_motorVals);

            DatagramPacket motorPacket = new DatagramPacket(motorBytes, motorBytes.length, _addr, _motorPort);

            try {
                _motorSocket.send(motorPacket);
            }
            catch (Exception e) {
                handleException(e);
            }

            try {
                byte [] telemetryBytes = new byte[88];
                DatagramPacket telemetryPacket = new DatagramPacket(telemetryBytes, telemetryBytes.length, _addr, _telemPort);
                _telemSocket.receive(telemetryPacket);
                double [] telemetryData = bytesToDoubles(telemetryBytes);
                System.out.printf("t: %6.3f | g: %+3.3f %+3.3f %+3.3f | q: %+3.3f %+3.3f %+3.3f %+3.3f | p: %+3.3f %+3.3f %+3.3f\n", 
                        telemetryData[0],
                        telemetryData[1], telemetryData[2], telemetryData[3],
                        telemetryData[4], telemetryData[5], telemetryData[6], telemetryData[7],
                        telemetryData[8], telemetryData[9], telemetryData[10]);
                count++;
            }
            catch (Exception e) {
                handleException(e);
            }

            yield();
        }

        _motorSocket.close();
        _telemSocket.close();

    } // run

    public void halt()
    {
        _running = false;
    }

    public void setMotors(double [] motorVals)
    {
        for (int i=0; i<motorVals.length; ++i) {
            _motorVals[i] = motorVals[i];
        }
    }

    private void construct(String host, int motorPort, int telemetryPort, int motorCount)
    {
        _motorPort = motorPort;
        _telemPort = telemetryPort;

        try {
            _addr = InetAddress.getByName(host);
            _motorSocket = new DatagramSocket();
            _telemSocket = new DatagramSocket(telemetryPort);
            _telemSocket.setSoTimeout(TIMEOUT);
        } 
        catch (Exception e) {
            handleException(e);
        }

        _motorVals = new double [motorCount];

        _running = false;
    }

    private int _motorPort;
    private int _telemPort;

    private double [] _motorVals;

    private boolean _running;

    InetAddress _addr;

    private DatagramSocket _motorSocket;
    private DatagramSocket _telemSocket;

    private static void handleException(Exception e)
    {
    }

    // Adapted from view-source:https://stackoverflow.com/questions/2905556/how-can-i-convert-a-byte-array-into-a-double-and-back

    private static byte[] doublesToBytes(double [] doubles)
    {
        int n = doubles.length;

        byte [] bytes = new byte[8*n];

        for (int i=0; i<n; ++i) {

            long l = Double.doubleToRawLongBits(doubles[i]);

            for (int j=0; j<8; ++j) {
                bytes[i*8+j] = (byte)((l >> (j*8)) & 0xff);
            }
        }

        return bytes;
    }

    private static double[] bytesToDoubles(byte [] bytes)
    {
        int n = bytes.length>>3;

        double [] doubles = new double [n];

        for (int i=0; i<n; ++i) {

            long bits = 0;

            int beg = 8 * i;

            for (int j=0; j<8; ++j) {
                bits = (bits << 8) | (bytes[beg+8-j-1] & 0xff);
            }

            doubles[i] = Double.longBitsToDouble(bits);
        }

        return doubles;
    }

    // Test code ========================================================

    public static void main(String [] args)
    {
        Multicopter copter = new Multicopter("127.0.0.1", 5000, 5001);

        copter.start();

        double [] motorVals = new double[4];

        try {
            for (int i=0; i<10; ++i) {

                Thread.sleep(2000);

                copter.setMotors(motorVals);

                for (int j=0; j<motorVals.length; ++j) {
                    motorVals[j] += 0.1;
                }
            }
        }
        catch (Exception e) {
        }

        copter.halt();
    }
}
